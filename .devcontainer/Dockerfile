FROM ghcr.io/xboxdev/nxdk:latest

# NOTE: curl-dev is require to workaround unknown option failure from libcurl (assumed is old version)
RUN apk add build-base ninja zip unzip curl-dev curl git

# Initial env variable for vcpkg path
ENV VCPKG_ROOT="/usr/local/vcpkg"
ENV PATH="${PATH}:${VCPKG_ROOT}"

# Install vcpkg
RUN git clone https://github.com/microsoft/vcpkg ${VCPKG_ROOT} \
    && ${VCPKG_ROOT}/bootstrap-vcpkg.sh

# Include missing path to nxdk's bin directory.
ENV PATH="${NXDK_DIR}/bin:${PATH}"
