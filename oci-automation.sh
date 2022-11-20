#!/bin/bash
clear
echo "Starting VM creation"
read -p "Enter VM hostname list in the next file [Enter to Continue]"
vim ./vm-list.txt
oci iam compartment list|egrep "name|\"id\":"|tr -d '"'|tr -d ','
read -p "Choose compartment ID to create VM's in:" Compartment_ID
oci network subnet list -c $Compartment_ID|egrep "\"id\":|display-name"|tr -d '"'|tr -d ','
read -p "Choose the subnet ID for the network vnic:" SUB_ID
oci compute image list -c $TENANT_ID --lifecycle-state AVAILABLE|egrep "display-name|\"id\":"|tr -d '"'|tr -d ','
read -p "[Not working currently] Choose base image to be used:" IMAGEID


echo "Please find the below list which will be created.."
cat ./vm-list.txt

echo -e "Please choose the Instance shape (applied to all above VMs)\n"

printf "\nName  Memory Constraints      OCPU Constraints\n
VM.PCAStandard1.1       16 GB   1       \n
VM.PCAStandard1.16      256 GB  16      \n
VM.PCAStandard1.2       32 GB   2       \n
VM.PCAStandard1.24      384 GB  24      \n
VM.PCAStandard1.32      512 GB  32      \n
VM.PCAStandard1.4       64 GB   4       \n
VM.PCAStandard1.48      768 GB  48      \n
VM.PCAStandard1.60      960 GB  60      \n
VM.PCAStandard1.8       128 GB  8       \n"

read -p "VM Shape: (example: VM.PCAStandard1.1) " vm_shape
echo -e "Value for shape: $vm_shape \n"
VM_SHAPE=$(echo "$vm_shape")
read -p "boot size: (example: 150) " bootsize
echo -e "Value for boot size: $bootsize \n"
BOOTSIZE=$(echo "$bootsize")
echo -e "\n Compartment_ID = $Compartment_ID\n subnet-ID = $SUB_ID\n Image-ID = $IMAGEID\n bootsize = $BOOTSIZE\n"
echo -e "\nEnter to continue"
read
read -p "Add block device? [Yes/No] " answerblk1
if [[ "$answerblk1" == "Yes" ]]
then
        echo "Adding block device"
        read -p "block name: (example: u01) " blockname
        echo -e "Value for blockname: $blockname \n"
        read -p "block size: (example: 150) " blocksize
        echo -e "Value for block size: $blocksize \n"
        BLOCKSIZE=$(echo "$blocksize")
else
        echo "Skipping"
fi
read -p "Add more block devices? [Yes/No] " answerblk

if [[ "$answerblk" == "Yes" ]]
then
        echo "Adding block device"
        read -p "block name: (example: u02) " blockname2
        echo -e "Value for blockname: $blockname2 \n"
        read -p "block size: (example: 150) " blocksize2
        echo -e "Value for block size: $blocksize2 \n"
        BLOCKSIZE2=$(echo "$blocksize2")
fi

for VMNAME in $(cat ./vm-list.txt)
do
        echo "Creating $VMNAME with the same hostname"
        BLOCKNAME=$(printf "$blockname";printf "_";printf "$VMNAME")
        oci compute instance launch --availability-domain ad1 \
                --compartment-id $Compartment_ID \
                --display-name $VMNAME \
                --shape $VM_SHAPE \
                --subnet-id $SUB_ID \
                --source-details '{"bootVolumeSizeInGBs":"200","imageId":"$IMAGE_ID","sourceType":"image"}' \
                --hostname-label $VMNAME \
                --assign-public-ip false \
                --ssh-authorized-keys-file ~/.ssh/id_rsa.pub > createoutput.json
        echo "checking the status of $VMNAME"
        grep "opc-work-request-id" createoutput.json |awk '{ print $2 }'|tr -d '"' |tr -d ' ' > ./workid.txt

        i=1
        while [ $i -eq 1 ]
        do
                oci work-requests work-request get --work-request-id $(cat ./workid.txt) |grep '"status":'|awk -F':' '{print $2}'|tr -d '"' |tr -d ',' |tr -d ' ' > ./work-status.json
                oci work-requests work-request get --work-request-id $(cat ./workid.txt) > ./work-status-output.json
                workIDstatus=$(grep "SUCCEEDED" ./work-status-output.json|wc -l)
                echo "Checking VM status"
                sleep 1
                if [[ $workIDstatus -eq "1" ]]
                then
                        echo "$(date) VM $VMNAME Created"
                        if [[ "$answerblk1" == "Yes" ]]
                        then
                                BLOCKNAME=$(printf "$blockname";printf "_";printf "$VMNAME")
                                vmID=`grep identifier work-status-output.json |awk -F':' '{print $2}'|tr -d '"'|tr -d ' '`
                                echo "Creating block $BLOCKNAME for $vmID"
                                oci bv volume create --display-name $BLOCKNAME --size-in-gbs $BLOCKSIZE --wait-for-state AVAILABLE --compartment-id $Compartment_ID --availability-domain ad1
                                VOCID=$(oci bv volume list --display-name $BLOCKNAME --compartment-id $Compartment_ID --lifecycle-state AVAILABLE|grep "\"id\":"|awk -F':' '{print $2}'|tr -d '"' |tr -d ','|tr -d ' ')
                                oci compute volume-attachment attach --instance-id $vmID --volume-id $VOCID --type paravirtualized --display-name $BLOCKNAME --wait-for-state ATTACHED
                        fi
                        if [[ "$answerblk" == "Yes" ]]
                        then
                                vmID=`grep identifier work-status-output.json |awk -F':' '{print $2}'|tr -d '"' |tr -d ' '`
                                echo -e "\nAdding block device:"
                                echo -e "Value for blockname: $blockname2 \n"
                                BLOCKNAME2=$(printf "$blockname2";printf "_";printf "$VMNAME")
                                echo -e "Value for block size: $blocksize2 \n"
                                echo -e "\n"
                                sleep 3
                                oci bv volume create --display-name $BLOCKNAME2 --size-in-gbs $BLOCKSIZE2 --compartment-id $Compartment_ID --wait-for-state AVAILABLE --availability-domain ad1
                                VOCID2=$(oci bv volume list --display-name $BLOCKNAME2 --compartment-id $Compartment_ID --lifecycle-state AVAILABLE|grep "\"id\":"|awk -F':' '{print $2}'|tr -d '"' |tr -d ','|tr -d ' ')
                                oci compute volume-attachment attach --instance-id $vmID --volume-id $VOCID2 --type paravirtualized --display-name $BLOCKNAME2 --wait-for-state ATTACHED
                        fi
                        i=0
                else
                        if [[ "$(cat ./work-status.json)" == "FAILED" ]]
                        then
                                echo "$VMNAME status: $(cat ./work-status.json)"
                                break
                                exit 1
                        else
                                echo "$VMNAME status: $(cat ./work-status.json)"
                                i=1
                        fi
                fi
        done
done

echo "VM IPs:"
echo '[hosts-py3]' > ./newest-vms
for i in $(cat ./vm-list.txt);do oci compute instance list-vnics --compartment-id $(grep compartment-id ./work-status-output.json|awk -F':' '{ print $2 }'|tr -d '"'|tr -d ','|xargs)|grep -C 10 $i|egrep "display-name|private-ip"|awk -F':' '{ print $2 }'|tr -d ','|xargs >> ./newest-vms;done

sed -i 's/ 10./ ansible_ssh_host=10./g' ./newest-vms
echo '[hosts-py3:vars]' >> ./newest-vms
echo 'ansible_python_interpreter=/usr/bin/python3' >> ./newest-vms
cat ./newest-vms
scp -p ./newest-vms ansible-vm:/home/ansible/automate/
echo "Running configs for the servers using Ansible"
read -p "Enter your username that will run the playbook on ansible: " USERNAMEANSIBLE
ssh -t ansible-vm "ansible-playbook -i /home/ansible/automate/newest-vms /home/ansible/automate/install-prereq.yml -Kb -u $USERNAMEANSIBLE"
