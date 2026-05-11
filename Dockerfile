# ETAP 1: Builder
FROM alpine:latest AS builder
# Instalacja kompilatora i narzędzi
RUN apk add --no-cache g++ make

WORKDIR /app
COPY main.cpp httplib.h ./

# Kompilacja aplikacji
RUN g++ main.cpp -o app -static -O3 -pthread


FROM alpine:latest

# Instalujemy curl
RUN apk add --no-cache curl

# Etykiety zgodne z OCI
LABEL org.opencontainers.image.authors="Tomasz Duchnik"
LABEL org.opencontainers.image.title="Weather App CPP"
LABEL org.opencontainers.image.description="Aplikacja pogodowa "
LABEL org.opencontainers.image.version="1.0.0"

WORKDIR /app
# Kopiujemy skompilowany plik z pierwszego etapu
COPY --from=builder /app/app /app/app

# Informacja o porcie
EXPOSE 8080

# HEALTHCHECK
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
  CMD curl -f http://localhost:8080/ || exit 1

# Uruchomienie aplikacji
CMD ["/app/app"]
