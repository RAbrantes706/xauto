<?PHP
$error_type = $_GET['e'];
switch ($error_type)
{
case '400':
  echo '<head><title>400 Bad Request</title></head>';
  echo '<h1>Bad Request</h1>';
  echo '<p>One or more required parameters were missing or invalid.</p>';
  break;
case '401':
  echo '<head><title>401 Unauthorized</title></head>';
  echo '<h1>Unauthorized</h1>';
  echo '<p>Authorization token does not exist.</p>';
  break;
case '402':
  echo '<head><title>402 Forbidden</title></head>';
  echo '<h1>Forbidden</h1>';
  echo '<p>Authorization token does not exist.</p>';
  break;
case '403':
  echo '<head><title>403 Forbidden</title></head>';
  echo '<h1>Forbidden</h1>';
  echo '<p>Request is missing valid credentials.</p>';
  break;
case '404':
  echo '<head><title>404 Not Found</title></head>';
  echo '<h1>Not Found</h1>';
  echo '<p>The requested URL was not found on this server.</p>';
  break;
case '405':
  echo '<head><title>405 Method not Allowed</title></head>';
  echo '<h1>Method not allowed</h1>';
  echo '<p>The method specified in the Request-Line is not 
    allowed for the resource identified by the Request-URI.</p>';
  break;
case '500':
  echo '<head><title>500 Internal Server Error</title></head>';
  echo '<body>';
  echo '<h1>Internal Server Error</h1>';
  echo '<p>The server encountered an unexpected condition which prevented it 
    from fulfilling the request.</p>';
  break;
}
?>
