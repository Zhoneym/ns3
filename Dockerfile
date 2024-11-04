FROM arm64v8/alpine:latest
RUN apk add --no-cache redis=7.4.1 keepalived
EXPOSE 6379
CMD ["sh", "-c", "redis-server & keepalived -n -D"]
