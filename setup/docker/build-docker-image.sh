# Use docker files provided by spark distribution. It assumes that spark has been
# unzipped in /opt/spark directory
repo=litendata
version=v3.3.2
# Use docker daemon inside the minikube
eval $(minikube docker-env)
# Go to opt/spark to build the images
pushd /opt/spark
# It creates an image with repo like litendata/spark:v3.3.2
./bin/docker-image-tool.sh -r docker.io/${repo} -t ${version} -f kubernetes/dockerfiles/spark/Dockerfile.java17 build
# Push this image into the repo, docker should have been logged in
# docker push ${repo}/spark:${version}
# Create a pyspark image named like litendata
./bin/docker-image-tool.sh -r docker.io/${repo} -t ${version} -p kubernetes/dockerfiles/spark/bindings/python/Dockerfile build
# Push this image into the repo
# docker push ${repo}/spark-py:${version}
