# Image de base légère de Node.js
FROM node:18.16.0-alpine3.17

RUN apk add tzdata
RUN apk add musl-locales

# Créé un répertoire de travail pour l'application et s'y déplace
WORKDIR /app

# Copie les fichiers package.json et package-lock.json (s'il existe)
COPY package*.json .

# Installe les dépendance de l'application
RUN npm install --omit=dev

# Copie l'application
COPY --chown=node:node . .

# Expose le port sur lequel l'applcation écoute
EXPOSE 8080

## L'utilisateur courant devient "node"
USER node

# Démarre l'application
CMD ["npm", "start"]
