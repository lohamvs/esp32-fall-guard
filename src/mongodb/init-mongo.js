db = db.getSiblingDB('sensor');

db.createUser({
  user: "sensorAdmin",
  pwd: "sensorPassword",
  roles: [
    { role: "dbOwner", db: "sensor" }
  ]
});