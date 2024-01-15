#!/bin/bash

# Set working directory to current script path
cd "$(dirname "$0")"

# Force TLS 1.2
export https_protocols=TLSv1.2

# Specify URL
baseUrl="https://test/BeyondTrust/api/public/v3/"

# API Key
apiKey="key"

# Username of BI user associated to the API Key
runAsUser="user"

# Build the Authorization header
headers=("Authorization: PS-Auth key=${apiKey}; runas=${runAsUser}")

# Verbose logging
verbose=true

# Sign in API with error handling
function sign_in() {
    if $verbose; then
        echo "Signing-in.."
    fi

    signInResult=$(curl -s -X POST "${baseUrl}Auth/SignAppIn" -H "${headers[@]}")

    if $verbose; then
        echo "..Signed-in as ${signInResult.UserName}"
        echo ""
    fi
}

# Exception handling function
function handle_exception() {
    echo ""
    echo "Exception:"
    
    if $verbose; then
        echo $1
        echo $1 | awk '{print $0}' ORS='\n'
    else
        echo $1 | awk '{print $1}'
        echo $1 | awk '{print $2}'
    fi
}

# Sign in with error handling
sign_in_result=$(sign_in) || handle_exception "$sign_in_result"

# File name
fileName="PasswordCache$(date +'%Y-%m-%d').txt"

# Remove file if exists
if [ -f "$fileName" ]; then
    rm "$fileName"
fi

# Get all managed accounts
allMa=$(curl -s "${baseUrl}ManagedAccounts")

# Iterate through accounts
for account in $(echo "$allMa" | jq -c '.[]'); do
    domainName=$(echo "$account" | jq -r '.DomainName')
    accountName=$(echo "$account" | jq -r '.AccountName')
    systemToFind=$(echo "$account" | jq -r '.SystemName')

    if [ -z "$domainName" ]; then
        accountToFind="$accountName"
    else
        accountToFind="$domainName\\$accountName"
    fi

    ma=$(curl -s "${baseUrl}ManagedAccounts?systemName=${systemToFind}&accountName=${accountToFind}")

    body=$(jq -n \
        --arg accessType 'View' \
        --arg systemId "$ma.SystemId" \
        --arg accountId "$ma.AccountId" \
        --arg durationMinutes '10' \
        --arg conflictOption 'reuse' \
        '{AccessType: $accessType, SystemId: $systemId, AccountId: $accountId, DurationMinutes: $durationMinutes, ConflictOption: $conflictOption}' | jq -c .)

    if [ -n "$domainName" ]; then
        systemToFind=""
    fi

    requestId=$(curl -s -X POST "${baseUrl}Requests" -d "$body" -H "Content-Type: application/json" | jq -r '.RequestId')

    managedAccountPassword=$(curl -s "${baseUrl}Credentials/$requestId")

    delimit='-------------------------------'

    echo "$delimit" >> "$fileName"
    echo "Managed Account Name = $accountName" >> "$fileName"
    echo "Domain if Domain user = $domainName" >> "$fileName"
    echo "Managed Account Password = $managedAccountPassword" >> "$fileName"
    echo "Managed System = $systemToFind" >> "$fileName"
    echo "$delimit" >> "$fileName"
done

