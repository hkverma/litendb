minikube start
eval $(minikube docker-env)
kubectl get pods -A
docker image ls
minikube addons enable metrics-server
# https://github.com/kubernetes/dashboard/blob/master/docs/user/access-control/README.md
# For now provide admin access to dashboard
#kubectl create -f dashboard-admin.yaml
#kubectl apply -f https://raw.githubusercontent.com/kubernetes/dashboard/v2.7.0/aio/deploy/recommended.yaml
#minikube dashboard &
#disown
