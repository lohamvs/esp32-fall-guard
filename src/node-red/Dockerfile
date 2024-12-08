FROM nodered/node-red:latest

WORKDIR /usr/src/node-red

RUN npm install node-red-node-mongodb node-red-dashboard

COPY flows.json /data/flows.json

EXPOSE 1880

CMD ["npm", "start", "--", "--userDir", "/data"]