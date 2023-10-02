#!/bin/bash


# Setting PATH for xml locations and guest images 

XML_DIR="/etc/libvirt/qemu"
GUEST_DIR="/Z-storage/KVM-store"
BKP_DIR="/tmp"

# XML will contain the dump of the guest images configurations
# GUEST_DIR will contain the .img files and other associated files (if available)


# get list of virtual guests (assuming proper permissions for the used user)
#

VM_GUESTS=$(ls /etc/libvirt/qemu|grep xml|sed 's/.xml//g')
#VM_GUESTS="as-test"

for vm in $VM_GUESTS
do

    echo "Deleting old backup.."
    rm -f $BKP_DIR/$vm.tar.gz
    echo "Getting XML path for $vm"
    VM_XML=$XML_DIR/$vm.xml
    sudo ls $VM_XML >/dev/null
    if [[ $? -eq 0 ]]
    then
        echo $VM_XML
    else
        echo "XML: Path $VM_XML is incorrect.. exiting"
        exit 1
    fi

    echo "Getting images path for $vm"
    VM_IMG=$(sudo virsh dumpxml as-bench|grep .img|awk -F"'" '{print $2}')
    #VM_IMG=$GUEST_DIR/$vm.img >/dev/null
    sudo ls $VM_IMG >/dev/null
    if [[ $? -eq 0 ]]
    then
        echo $VM_IMG
    else
        echo "IMG: Path $VM_IMG is incorrect.. exiting"
        exit 1
    fi

    echo "Creating tarball of $vm"

    tar czfPv $BKP_DIR/$vm.tar.gz $VM_IMG $VM_XML

    if [[ $? -eq 0 ]]
    then
        echo "Backup of $vm is successful, path: $BKP_DIR/$vm.tar.gz"
    else
        echo "Error in creating tarball.. exiting"
        exit 1
    fi
done




