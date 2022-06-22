sed -i s/_SERVERNAME_/$SERVER_DOMAIN/g /etc/apache2/sites-available/000-default.conf
sed -i s/_SERVERNAME_/$SERVER_DOMAIN/g /etc/apache2/apache2.conf
sed -i s/_SERVERNAME_/$SERVER_DOMAIN/g /etc/apache2/apache2.conf
sed -i s/_NAME_/$HEADER_NAME/g /var/www/html/index.html
