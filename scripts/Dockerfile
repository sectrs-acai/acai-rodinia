FROM --platform=linux/amd64 ubuntu:20.04

RUN mkdir -p /scripts
ADD docker-ubuntu-packages.sh /scripts/
ADD docker-download-toolchain.sh /scripts/
ADD docker-install-cuda.sh  /scripts/

RUN chmod 1777 -R /tmp
RUN apt-get -q update >/dev/null
RUN apt-get install -y sudo
RUN /scripts/docker-ubuntu-packages.sh
RUN /scripts/docker-download-toolchain.sh
RUN /scripts/docker-install-cuda.sh

VOLUME /armcca
WORKDIR /armcca

RUN adduser --disabled-password --gecos '' armcca
RUN adduser armcca sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

USER armcca

ENV PATH="${PATH}:/scripts/aarch64-none-linux-gnu/bin:/usr/local/cuda/bin"
