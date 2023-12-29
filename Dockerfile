FROM node
WORKDIR /usr/src/app
COPY . .
RUN apt-get update
RUN apt-get install -y ffmpeg
RUN npm install
EXPOSE 3333
CMD ["npx", "bajel"]
