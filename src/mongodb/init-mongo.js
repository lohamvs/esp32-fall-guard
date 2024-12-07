db.createUser({
    user: "admin",
    pwd: "password",
    roles: [{ role: "readWrite", db: "mqtt_data" }]
  });
  