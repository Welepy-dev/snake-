FROM gcc:latest

# Install raylib and dependencies
RUN apt-get update && \
    apt-get install -y git make cmake libgl1-mesa-dev libx11-dev libxcursor-dev \
    libxrandr-dev libxi-dev libxinerama-dev libxxf86vm-dev \
    gdb

WORKDIR /app

# Copy files into container
COPY . .

# Build using Makefile
RUN make

# Run the correct binary
CMD ["gdb", "-ex", "run", "./flocksim"]

