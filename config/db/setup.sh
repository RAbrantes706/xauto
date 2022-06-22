#!/bin/bash

sed s/_DATABASENAME_/$DATABASE_NAME/g /home/createStructure.sql > /tmp/1

sed s/_DATABASENAME_/$DATABASE_NAME/g /home/createUser.sql > /tmp/2
sed s/_DATABASEUSER_/$DATABASE_USER/g /tmp/2 > /tmp/3
sed s/_DBUSERPASSWORD_/$DATABASE_PASSWORD/g /tmp/3 > /tmp/4

cat /tmp/1 > /home/createStructure.sql
cat /tmp/4 > /home/createUser.sql

sleep 5

mariadb -u root -p$MYSQL_ROOT_PASSWORD < /home/createStructure.sql
mariadb -u root -p$MYSQL_ROOT_PASSWORD < /home/createUser.sql
