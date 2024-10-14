FROM gentoo/stage3:latest

RUN echo "Updating Gentoo index registry..."
RUN emerge-webrsync


RUN echo "Setting up package manager"
# Allow emerge newer versions of cmake. We need it because vcpkg wants it.
RUN echo "dev-build/cmake ~amd64" >> /etc/portage/package.accept_keywords/unmask
# # Get some packages as binaries and don't compile them. Actually, it allows to not compile only zip :(
# RUN rm -rf /etc/portage/binrepos.conf/*
# RUN echo -e "[binhost]\npriority = 9999\nsync-uri = https://gentoo.osuosl.org/experimental/amd64/binpkg/default/linux/17.1/x86-64/" >> /etc/portage/binrepos.conf/osuosl.conf
# RUN cat /etc/portage/binrepos.conf/*
# Arguments that going to be added automatically to `emerge` command. `man emerge` in help. Or read https://wiki.gentoo.org/wiki/Binary_package_guide/en#Installing_binary_packages .
# RUN echo 'EMERGE_DEFAULT_OPTS="${EMERGE_DEFAULT_OPTS} --binpkg-respect-use=y --getbinpkg=y --verbose --verbose-conflicts "' >> /etc/portage/make.conf

RUN echo "Installing dependencies..."

RUN emerge --quiet --verbose --tree --verbose-conflicts --jobs=2 dev-vcs/git dev-build/cmake zip

# RUN echo "Installing vcpkg..."
# RUN git clone https://github.com/Microsoft/vcpkg.git && \
# 		./vcpkg/bootstrap-vcpkg.sh -disableMetrics 
#
# COPY ./vcpkg.json ./vcpkg.json
#
# RUN echo "Installing dependencies using vcpkg"
# RUN ./vcpkg/vcpkg install

WORKDIR /backend

RUN echo "Copy sources of the project"
COPY . .
RUN ls -lah

# expected to be mounted at runtime as volume.
# COPY resources/ ./resources/ 


RUN echo "Configuring project..."
RUN cmake -S ./ -B ./build --log-level DEBUG


# RUN echo "Building project..."
# RUN cmake --build ./build --parallel $(nproc) --verbose

RUN echo "really installing"
RUN cmake --install ./build

RUN echo "checking cmake-cmake"
RUN cmake -S. -B build2 -DMAGIC_ENUM_OPT_TEST_INSTALLED_VERSION=1 --log-level=DEBUG
RUN cmake --build build2 -j $(nproc) --verbose

RUN echo "checking pkgconfig-cmake"
RUN cat "/usr/local/share/pkgconfig/magic_enum.pc"
RUN cmake -S. -B build3 -DMAGIC_ENUM_OPT_TEST_INSTALLED_VERSION_PKGCONFIG=1 --log-level=DEBUG
RUN cmake --build build3 -j $(nproc) --verbose



# RUN echo "what's with linking?"
# RUN ls -lahR /usr/local/lib64  
# RUN lddtree /usr/local/lib64/libArby.so
# RUN lddtree /usr/local/bin/Arby

# CMD ./build/Backend





