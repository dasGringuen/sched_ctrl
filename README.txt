Implementation of an idea of Daniel Gutson

Install:
==========

tested on Debian 8. Should work on Ubuntu

sudo apt-get install linux-headers-$(uname -r)
sudo ln -s /usr/src/linux-headers-$(uname -r) /usr/src/linux 


Module:
=======
cd module
make
./run

User space:
===========

cd driver_test
make

Test with 3 threads:
====================
should get something like:

adrian@debian: ~/sched_ctrl/driver_test$ ./driver_test 
Starting test... 
Father:6671
t1: pid 6672
t2: pid 6673
t3: pid 6674

T1_1T1_1T1_1T1_1T1_1T1_1T1_1T1_1T1_1T1_1
T2T2T2T2T2T2T2T2T2T2
T3T3T3T3T3T3T3T3T3T3T3
T1_2T1_2T1_2T1_2T1_2T1_2T1_2T1_2T1_2T1_2
