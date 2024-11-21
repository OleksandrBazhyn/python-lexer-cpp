FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++ \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN mkdir build

WORKDIR /app/build
RUN cmake ..
RUN make

CMD ["./PythonLexer"]
