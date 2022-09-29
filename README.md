# BEMDV: Blockchain Enabled Emergency Message Dissemination for Connected Vehicles


Installation of OMNeT++, SUMO and VEINS

This section provides step by step installation process of OMNeT++, SUMO and VEINS to run a basic
simulation in Ubuntu 16.04.
Setting up environment for Smooth installation of omnet++ 5.1.1
First of all check python version , omnet run smoothly with python2.7 instead of 3.x, type &quot;python&quot; in
terminal to check version Ubuntu comes installed with python2.7
Secondly, install java if not installed already with this command:

sudo apt-get install openjdk-8-jdk

download omnet++ 5.1.1 archive from internet and extract it in home/&lt;user&gt;/directory
Link to download omnet++ 5.1.1

https://www.omnetpp.org/component/jdownloads/category/32-release-older-versions?Itemid=-1

After extracting OMNeT, enter into OMNeT directory by terminal by typing“cd omnetpp-5.1.1” assuming
that you are already in the user directory in terminal.

type pwd and copy directory now type the following command to set paths
gedit ∼/.bashrc

A file will open ,now we have to set path : (write this code with your respective directory and add /bin at
the end)
export PATH=$PATH:/home/ubuntu/omnetpp-5.1.1/bin

export OMNET DIR=/home/ubuntu/omnetpp-5.1.1

(after writing to the file , save it and close it)
after saving, type following command in terminal:

source ∼/.bashrc

now type the following command to install the necessary libraries for OM-NeT , without these packages
./configure command would give error.

sudo apt-get install build-essential gcc g++ bison flex perl tcl-dev tk-dev libxml2-dev zlib1g-dev default-
jre doxygen graphviz libwebkitgtk-1.0-0 qt4-qmake libqt4-dev libqt4-opengl-dev openscenegraph
libopenscenegraph-dev openscenegraph-plugin-osgearth osgearth osgearth-data libosgearth-dev
openmpi-bin libopenmpi-dev nemiver qt5-default

then type the following command in terminal, if everything goes well, after configuration &quot;good&quot; will be
show after execution

./configure
then type the following command, this will take some time so be patient.
Type “make”
after “make” command is completed, type following to run OMNeT++ 5.1.1
omnetpp
After typing “omnetpp”, OMNeT++ will be opened.

INET Installation
after running omnetpp , install INET framework , if it doesn’t show up by default then go to “help” menu
and click on “install simulation models” and install INET framework after installing INET framework, run
“build all” from “project” menu
Installing Sumo and its packages
after building all, download the following packages from
“http://sumo.dlr.de/wiki/Contributed/SUMOPy#Linux” with the help of “sudo apt-get install &lt;package&gt;”
python-numpy

python-wxgtk2.8
python-opengl
python-imaging
python-matplotlib
python-mpltoolkits.basemap

as mentioned on the site above , downloading python-wxgtk2.8 on Ubuntu 16.04 may give error if so
then use the following commands for installing it.

sudo add-apt-repository ppa:nilarimogard/webupd8
sudo apt-get update
sudo apt-get install python-wxgtk2.8

this will install python-wxgtk2.8
after this we will install sumo using the terminal command that is:

sudo apt-get install sumo

this will install sumo

Importing Veins and Running Example
next step is to import veins 4.5 in omnetpp project

first of all download veins 4.5 from “http://veins.car2x.org/download/” and extract it to the same
folder/directory where omnetpp is extracted after extracting veins 4.6 , we need to import it in omnetpp
, click on “file” then click “import”, after that select “general” and then “existing projects into
workspace” and click next.

select root directory by clicking on browse button and add vein 4.5 folder and click finish. Now we need
to build all project once again by clicking project→ build all.

after building open new terminal and drag “sumo launchd.py” to the terminal,it will write the path to
that file itself, then add “-vv -c”.
now search for “sumo-gui” in “computer” folder and drag it into the terminal ,now your command
would look something like this.
“/home/sarmad/veins-veins-4.5/sumo-launchd.py’ -vv -c ’/usr/bin/sumo-gui”

press enter to execute the above command.
after running the above command , it will say “Logging to /tmp/sumo-launchd.log Listening on port
9999” now go into omnetpp and right click on omnetpp.ini which lies inside the
veins→examples→veins→omnetpp.ini and “run as”→“omnet++ simulation” Thats it.
