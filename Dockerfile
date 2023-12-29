FROM node
WORKDIR /usr/src/app
COPY . .
RUN npm install
EXPOSE 3333
CMD ["npx", "bajel"]
