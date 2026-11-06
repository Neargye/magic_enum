FROM gentoo/stage3:latest@sha256:e54c014546d9015a83577160514c16d4709d0c7eb792df4da2e3fb70b16197dc

RUN emerge-webrsync \
    && echo "dev-build/cmake ~amd64" > /etc/portage/package.accept_keywords/magic_enum \
    && emerge --quiet --verbose --jobs=2 dev-build/cmake virtual/pkgconfig

WORKDIR /src
COPY . .
RUN cmake -S . -B build-install \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/tmp/configured-install \
      -DMAGIC_ENUM_OPT_BUILD_EXAMPLES=OFF \
      -DMAGIC_ENUM_OPT_BUILD_TESTS=OFF \
      -DMAGIC_ENUM_OPT_VERIFY_INTERFACE_HEADER_SETS=ON \
    && cmake --build build-install --target magic_enum_verify_interface_header_sets --parallel "$(nproc)" \
    && cmake --install build-install --prefix /tmp/magic_enum \
    && cmake -S . -B build-installed-cmake \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH=/tmp/magic_enum \
      -DMAGIC_ENUM_OPT_TEST_INSTALLED_VERSION=ON \
    && cmake --build build-installed-cmake --parallel "$(nproc)" \
    && ctest --test-dir build-installed-cmake --output-on-failure --no-tests=error \
    && cmake -S . -B build-installed-pkgconfig \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH=/tmp/magic_enum \
      -DMAGIC_ENUM_OPT_TEST_INSTALLED_VERSION_PKGCONFIG=ON \
    && cmake --build build-installed-pkgconfig --parallel "$(nproc)" \
    && ctest --test-dir build-installed-pkgconfig --output-on-failure --no-tests=error
