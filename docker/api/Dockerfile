FROM ubuntu:focal

ENV DEBIAN_FRONTEND=noninteractive

#RUN apt update && apt -y upgrade
#RUN apt install -y software-properties-common

#RUN add-apt-repository ppa:certbot/certbot
RUN apt update --fix-missing && apt -y upgrade
RUN apt install -y apache2 php7.4 php7.4-mysql php7.4-json libapache2-mod-php7.4

# Enable apache mods.
RUN a2enmod php7.4
RUN a2enmod ssl
RUN a2enmod rewrite

# Update the PHP.ini file, enable <? ?> tags and quieten logging.
RUN sed -i "s/short_open_tag = Off/short_open_tag = On/" /etc/php/7.4/apache2/php.ini
RUN sed -i "s/error_reporting = .*$/error_reporting = E_ERROR | E_WARNING | E_PARSE/" /etc/php/7.4/apache2/php.ini

# Manually set up the apache environment variables
ENV APACHE_RUN_USER www-data
ENV APACHE_RUN_GROUP www-data
ENV APACHE_LOG_DIR /var/log/apache2
ENV APACHE_LOCK_DIR /var/lock/apache2
ENV APACHE_PID_FILE /var/run/apache2.pid

# Copy this repo into place.
ADD client/ /var/www/html/
ADD api/   /var/www/html/api/
ADD certificate/*.pem /etc/apache2/certificate/
ADD ./.env /var/www/html/api/src/
ADD vendor/ /var/www/html/api/src/vendor/
ADD favicon.ico /var/www/html/
ADD logo.png /var/www/html/

# Update the default apache site with the config
ADD config/httpd/apache2.conf /etc/apache2/
ADD config/httpd/apache-config.conf /etc/apache2/sites-available/000-default.conf

COPY config/httpd/docker-entrypoint.sh /usr/local/bin/
COPY config/httpd/setup.sh /home/
ENTRYPOINT ["docker-entrypoint.sh"]

EXPOSE 80
EXPOSE 443
