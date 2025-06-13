# Intro

- This projects aims to implement the technique discussed in the Research paper released by Microsoft [link](https://www.microsoft.com/en-us/research/publication/subvirt-implementing-malware-with-virtual-machines/). Basically, the rootkit inserts a thin layer of hypervisor between the host-OS and the hardware, to do this I plan to use vt-x and monitor system-exit calls to get important function calls.
- More information about this will be published in my blog later... :)

## Currently Implemented
1. Process Hiding
2. Synchroniztion
3. Process capabilites, bit buggy tho..
   
# To-Do :

1. Process Elevation (DKOM)✅
2. SubVirt Module
3. Network Communication/Dropper
4. better anti-anti-rootkit evasion techniques(?)
5. GUI client(?)

## <del>THIS PROJECT HAS YET TO BE COMPLETED, DO NOT RUN IT!!!!</del>
## Completed Process Hiding...🙌
