# ReliableUdpVideoStream-ns3

## How to test
Using bind mount feature of docker, you can synchronize directories on the host and container.
1. Clone this repository
2. Run ns-3 container with bind mount option `-v` to bind `local_path_to_this_repo/scratch` directory to `scratch` directory in the container. Note that both paths should be absolute path. 
3. Run ./waf in the container to build files in scratch directory.
