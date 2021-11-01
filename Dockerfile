FROM gcc:9.4.0
WORKDIR /home
RUN apt-get update && apt-get upgrade -y
EXPOSE 3000

#FROM alpine:3.14
#WORKDIR /home
#RUN apk add --no-cache gcc musl-dev
