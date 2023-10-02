#!/bin/bash


# Setting PATH for xml locations and guest images 

XML_DIR="/etc/libvirt/qemu/"
GUEST_DIR="/Z-storage/KVM-store/"

# XML will contain the dump of the guest images configurations
# GUEST_DIR will contain the .img files and other associated files (if available)


# get list of virtual guests (assuming proper permissions for the used user)
#

VM_GUESTS=$(virsh list --all)
echo $VM_GUESTS




