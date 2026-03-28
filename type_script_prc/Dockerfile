FROM node:22-alpine
WORKDIR /app
COPY package*.json ./
RUN npm ci
COPY . .
RUN npm i -D tsx
RUN npx tsc
CMD ["node", "dist/server_express.js"]