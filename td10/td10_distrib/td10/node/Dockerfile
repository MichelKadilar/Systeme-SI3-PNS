# Image de base légère de Node.js
FROM node:18.16.0-alpine3.17

# Créé un répertoire de travail pour l'application et s'y déplace
WORKDIR /app

# Copie les fichiers package.json et package-lock.json (s'il existe)
COPY package*.json .

# Installe les dépendance de l'application
RUN npm install --omit=dev

# Copie l'application
COPY . .

# Expose le port sur lequel l'applcation écoute
EXPOSE 8080

# Démarre l'application
CMD ["npm", "start"]
