FROM golang:1.21-alpine AS builder

RUN apk add wget intltool libtool autoconf automake clang gmp-dev mpfr-dev curl-dev icu-dev libxml2-dev  make
RUN wget https://github.com/Qalculate/libqalculate/releases/download/v4.9.0/libqalculate-4.9.0.tar.gz
RUN tar zxvf libqalculate-4.9.0.tar.gz

WORKDIR libqalculate-4.9.0
RUN ./configure --disable-insecure --without-gnuplot-call --prefix=/usr
RUN make -j
RUN make install

WORKDIR /app
COPY go.mod go.sum main.go qalc.cpp qalc.h ./

ENV CXX=clang++
ENV CC=clang
RUN go mod download
RUN go build -trimpath -o /dist/app

RUN adduser -S qalcbot

RUN mkdir -p /dist/lib
RUN mkdir -p /dist/etc
RUN mkdir -p /dist/usr/share
RUN mkdir -p /dist/home/qalcbot
RUN chown qalcbot /dist/home/qalcbot

RUN ldd /dist/app | tr -s [:blank:] '\n' | grep ^/ | xargs -I % install -D % /dist/%
RUN ldd /usr/bin/qalc | tr -s [:blank:] '\n' | grep ^/ | xargs -I % install -D % /dist/%

RUN cp /etc/passwd /dist/etc/
RUN cp -rf /etc/ssl /dist/etc/
RUN cp -rf /usr/share/qalculate /dist/usr/share/

FROM scratch
COPY --from=builder /dist /
USER qalcbot
ENTRYPOINT ["/app"]