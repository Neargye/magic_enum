FROM gentoo/stage3:latest

RUN emerge-webrsync \
    && echo "dev-build/cmake ~amd64" > /etc/portage/package.accept_keywords/magic_enum \
    && emerge --quiet --verbose --jobs=2 dev-build/cmake virtual/pkgconfig

WORKDIR /src
COPY . .
RUN bash ./test_installed_version.bash
