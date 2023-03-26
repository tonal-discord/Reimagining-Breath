# Got program to launch once on windows, but now it's not working.
# docker build -t lvm .
# Ran it through docker desktop application (under images tab)
FROM fedora:37

# ARG DEBIAN_FRONTEND=noninteractive
# ENV TZ=America/New_York

# RUN dnf install -y https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm
RUN dnf install -y opencv qtcreator


# WORKDIR /usr/src/qtcreator
# RUN curl -o qt-creator8.0.1.run https://qt.mirror.constant.com/official_releases/qtcreator/8.0/8.0.1/qt-creator-opensource-linux-x86_64-8.0.1.run && \
#     chmod +x qt*.run

ENV DISPLAY=host.docker.internal:0.0

COPY . /usr/src/live-video-magnification

# RUN pcmanfm
CMD /usr/src/live-video-magnification/build-rvm-Desktop-Debug/rvm
# CMD xterm

# RUN ./qt-creator8.0.1.run
# CMD pcmanfm

# WORKDIR /usr/bin

# CMD [ "ls" ]
