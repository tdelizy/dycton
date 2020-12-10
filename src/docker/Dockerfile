#
# 	Docker File for Dycton Project
#
# This dockerfile builds a docker image for the Dycton project build system.
# sources and doc can be found here : https://gitlab.inria.fr/citi-lab/dycton
#
# why a build system inside a docker container :
# https://www.rainerhahnekamp.com/en/docker-build-environment/
#
#
#
# DOCKERFILE MANUEL : https://docs.docker.com/engine/reference/builder/



#===============================================================================
# STAGE 1 : BASE IMAGE BUILD-UP
#===============================================================================
# https://hub.docker.com/
# tried with alpine for a lighter image, but CPLEX refuses to install on that distro
FROM ubuntu:latest as base-img-dbg
MAINTAINER Tristan Delizy <delizy.tristan@gmail.com>

#-------------------------------------------------------------------------------
# User and system setup
#-------------------------------------------------------------------------------
RUN apt-get update && apt-get install -y dialog apt-utils sudo

ENV USERNAME=build
ENV HOMEDIR=/home/build

# ARG allow user building the image to specify a value for the build of the container
ARG UID

# a nice guard
# create user and homedir
RUN test -n "$UID" || (echo 'UID not set. Please run docker build with --build-arg UID=$UID' && false) \
 		&& useradd -rm -u $UID $USERNAME

# allow sudo usage for our user
RUN adduser $USERNAME sudo \
    && echo 'build ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers \
    && echo "alias ls='ls --color'" >> $HOMEDIR/.bashrc

# setup the user the docker will log in
USER $USERNAME
WORKDIR $HOMEDIR

# even with "-y" some installers may try to speak to us (and we don't want to)
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=America/New_York


#-------------------------------------------------------------------------------
# Packages install
#-------------------------------------------------------------------------------
RUN sudo ln -fs /usr/share/zoneinfo/Etc/GMT$offset /etc/localtime \
		&& DEBIAN_FRONTEND=noninteractive sudo apt-get install -y \
		build-essential \
		wget


#===============================================================================
# STAGE 2 : CROSSCOMPILATION TOOLCHAIN BUILD
#===============================================================================
FROM base-img-dbg as crosscompiler-builder

#-------------------------------------------------------------------------------
# packages required
#-------------------------------------------------------------------------------
RUN sudo ln -fs /usr/share/zoneinfo/Etc/GMT$offset /etc/localtime \
		&& DEBIAN_FRONTEND=noninteractive sudo apt-get install -y \
		autoconf \
		libtool \
		libtool-bin \
		gperf \
		git \
		gcc \
		make \
		bison \
		flex \
		texinfo \
		help2man \
		libncurses5-dev \
		gawk \
		unzip \
		zlibc \
		zlib1g \
		zlib1g-dev

#-------------------------------------------------------------------------------
# Tool setup : crosstool-ng
#-------------------------------------------------------------------------------
# build the 1.24 version from sources to avoid build problems
# maybe in future releases we could use precompiled version of this tool
RUN wget http://crosstool-ng.org/download/crosstool-ng/crosstool-ng-1.24.0.tar.bz2 \
		&& tar xjf crosstool-ng-1.24.0.tar.bz2 \
		&& cd crosstool-ng-1.24.0 \
		&& mkdir ~/ct-ng/ \
		&& ./configure --prefix=$HOMEDIR/ct-ng/ \
		&& make \
		&& make install \
		&& cd .. \
		&& rm crosstool-ng-1.24.0.tar.bz2

# path setup
ENV PATH="${PATH}:${HOMEDIR}/ct-ng/bin"


#-------------------------------------------------------------------------------
# Cross-compilation toolchain build
#-------------------------------------------------------------------------------
# add conig file (it describes the target toolchain)
ADD .config $HOMEDIR

# crosstool-ng crosscompilation toolchain build: mispel-unknown-elf
RUN ct-ng build


#===============================================================================
# STAGE 3 : DYCTON BUILD SYSTEM SETUP
#===============================================================================
FROM base-img-dbg as target


#-------------------------------------------------------------------------------
# packages required
#-------------------------------------------------------------------------------
# python3-dev \
# python3-distutils \
# gawk \
RUN sudo apt-get install -y \
		openjdk-8-jdk \
		python-dev \
		python-matplotlib \
		python-numpy \
		python-tk

#-------------------------------------------------------------------------------
# SystemC install
#-------------------------------------------------------------------------------
ENV SYSTEMCVERSION=systemc-2.3.1a
RUN wget https://www.accellera.org/images/downloads/standards/systemc/$SYSTEMCVERSION.tar.gz \
		&& tar xzvf $SYSTEMCVERSION.tar.gz \
		&& cd $SYSTEMCVERSION \
		&& mkdir obj \
		&& cd obj \
		&& ls .\
		&& ../configure \
		&& make \
		&& make install \
		&& cd .. \
		&& rm -R obj/ \
		&& cd .. \
		&& ls -la . \
		&& rm $SYSTEMCVERSION.tar.gz

ENV SYSTEMCROOT="${HOMEDIR}/$SYSTEMCVERSION"

#-------------------------------------------------------------------------------
# Cross-compilation toolchain install
#-------------------------------------------------------------------------------
# get crosscompilation toolchain binaries from previous stage
COPY --from=crosscompiler-builder ${HOMEDIR}/x-tools $HOMEDIR

# add toolchain to path
ENV PATH="${PATH}:${HOMEDIR}/mipsel-unknown-elf/bin"


#-------------------------------------------------------------------------------
# Cplex install
#-------------------------------------------------------------------------------
ARG CPLEX_INSTALLER
ADD $CPLEX_INSTALLER $HOMEDIR
ADD cplex_silent_intall.properties $HOMEDIR
RUN mkdir $HOMEDIR/cplex \
 		&& test -n "$CPLEX_INSTALLER" || (echo 'CPLEX_INSTALLER not set. Please run docker build with --build-arg CPLEX_INSTALLER=cplex_intaller_filename' && false) \
 		&& sudo chmod +x $CPLEX_INSTALLER \
		&& ./$CPLEX_INSTALLER -f "./cplex_silent_intall.properties" \
    && cd $HOMEDIR/cplex/python \
		&& sudo python setup.py install \
 		&& rm $HOMEDIR/$CPLEX_INSTALLER \
		&& sudo apt-get remove -y openjdk-8-jdk \
		&& sudo apt-get autoremove -y \
		&& sudo apt-get clean -y

#-------------------------------------------------------------------------------
# Container configuration
#-------------------------------------------------------------------------------
# create directory for volume mount (accessing dycton repository from inside the container)
RUN mkdir /home/build/mounted_repository \
    && echo "case \"$TERM\" in \n\
			xterm*|rxvt*) \n\
    		PS1=\"\[\e]0;${debian_chroot:+($debian_chroot)}\u@\h: \w\a\]$PS1\" \n\
    		;; \n\
				*) \n\
    		;; \n\
		esac \n\
		PS1='${debian_chroot:+($debian_chroot)}\e[7m\[\033[01;34m\]\u\e[27m@\e[32m\e[7mDycton_Build_Env(docker)\[\033[00m\]\e[27m\w\$ '" >> $HOMEDIR/.bashrc

CMD ["/bin/bash"]
WORKDIR /home/build/mounted_repository


#-------------------------------------------------------------------------------
# check install (dbg)
#-------------------------------------------------------------------------------
# RUN wget https://gitlab.inria.fr/citi-lab/dycton/-/archive/master/dycton-master.tar.bz2 \
# 		&& tar xjf dycton-master.tar.bz2 \
# 		&& cd dycton-master/src/platform_tlm/iss \
# 		&& make -B DY_SOFT=json_parser DY_ARCH=0 \
# 		&& ./run.x
