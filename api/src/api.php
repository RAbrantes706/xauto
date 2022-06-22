<?PHP
require 'vendor/autoload.php';
use ReallySimpleJWT\Token;

require_once 'User.php';
require_once 'Vehicle.php';
require_once 'Sensor.php';
require_once 'Measurement.php';

//Load environment variables
require 'DotEnv.php';
(new DotEnv('.env'))->load();

$uri = $_SERVER['REQUEST_URI'];
$query = explode('?', $uri);
$query = explode('/', $query[0]);

if(count($query)<3)
{
  header('HTTP/1.1 404 Not Found');
  $_GET['e'] = 404; include 'err.php';
  die();
}

switch ($query[2])
{
case 'users':
  if(!isset($query[3]) || empty($query[3]))
  {
    // .../api/users/
    switch ($_SERVER['REQUEST_METHOD'])
    {
    case 'GET':
      //Find out if the given user exists
      if(isset($_GET['user']))
      {
        $user = User::findUserByName($_GET['user']);
        $result['exist'] = $user ? true : false;
        header('HTTP/1.1 200 OK');
        echo json_encode($result);
      }
      elseif(isset($_GET['email']))
      {
        $user = User::findUserByEmail($_GET['user']);
        $result['exist'] = $user ? true : false;
        header('HTTP/1.1 200 OK');
        echo json_encode($result);
      }
      else
      {
        header("HTTP/1.1 400 Bad Request");
        $_GET['e'] = 400; include 'err.php';
      }
      break;
    case 'POST':
      //Create new user and send new resource location
      $data = json_decode(file_get_contents('php://input'));
      $newId = User::createUser($data->name, $data->password, $data->email);
      if($newId>0){
        header('HTTP/1.1 201 Created');
        header('Content-Location: /api/users/' . $newId);
      }else{
        header("HTTP/1.1 400 Bad Request");
        $_GET['e'] = 400; include 'err.php';
      }
      break;
    default:
      header("HTTP/1.1 405 Method not Allowed");
      $_GET['e'] = 405; include 'err.php';
    }
  }
  else
  {
    // .../api/users/*
    $userID = intval($query[3]);
    if(!User::exists($userID))
    {
      header('HTTP/1.1 401 Forbidden');
      $_GET['e'] = 401; include 'err.php';
      die();
    }

    if(!($token = getBearerToken()))
    {
      header('HTTP/1.1 401 Unauthorized');
      $_GET['e'] = 401; include 'err.php';
      die();
    }

    $valid = Token::validate($token, $_ENV['JWT_SECRET']) && 
      Token::validateExpiration($token, $_ENV['JWT_SECRET']);

    $payload = Token::getPayload($token, $_ENV['JWT_SECRET']);

    if(!$valid || $payload['user_id'] != $userID)
    {
      header('HTTP/1.1 403 Forbidden');
      $_GET['e'] = 403; include 'err.php';
      die();
    }

    if(!isset($query[4]) || empty($query[4]))
    {
      switch ($_SERVER['REQUEST_METHOD']){
      case 'GET':
        //Return user info JSON 
        $user = User::getUser($userID);
        header('HTTP/1.1 200 OK');
        echo json_encode($user);
        break;
      case 'PUT':
        //Update user data 
        $data = json_decode(file_get_contents('php://input'));
        User::updateUser($userID, $data->name, $data->password, $data->email);
        header('HTTP/1.1 204 No Content');
        header('Content-Location: /api/users/' . $userID);
        break;
      case 'DELETE':
        //Delete user
        User::deleteUser($userID);
        header('HTTP/1.1 204 No Content');
        break;
      default:
        header("HTTP/1.1 405 Method not Allowed");
        $_GET['e'] = 405; include 'err.php';
      }
    }
    elseif($query[4]=='vehicles')
    {
      // .../api/users/*/vehicles
        switch ($_SERVER['REQUEST_METHOD']){
        case 'GET':
          //Get user vehicle list
          $vehicles = Vehicle::getUserVehicles($userID);
          header('HTTP/1.1 200 OK');
          echo json_encode($vehicles);
          break;
        case 'POST':
          //Create a new vehicle for this user 
          $data = json_decode(file_get_contents('php://input'));
          $newId = Vehicle::createVehicle($userID, $data->name, $data->color);
          header('HTTP/1.1 201 Created');
          header('Content-Location: /api/vehicles/' . $newId);
          break;
        default:
          header("HTTP/1.1 405 Method not Allowed");
          $_GET['e'] = 405; include 'err.php';
        }
    }
    else
    {
      header('HTTP/1.1 404 Not Found');
      $_GET['e'] = 404; include 'err.php';
    }
  }
  break;
case 'vehicles':
  if(!isset($query[3]) || empty($query[3]))
  {
    header("HTTP/1.1 404 Not Found");
    $_GET['e'] = 404; include 'err.php';

    /* OLD CODE --> LIST ALL THE VEHICLES (has no use)
    // .../api/vehicles
    if($_SERVER['REQUEST_METHOD'] == 'GET')
    {
      //Return list of vehicles
      $vehicles = Vehicle::getAllVehicles();
      header('HTTP/1.1 200 OK');
      echo json_encode($vehicles);
    }
    else
    {
      header("HTTP/1.1 405 Method not Allowed");
      $_GET['e'] = 405; include 'err.php';
    }
     */
  }
  else
  {
    $vehicleID = intval($query[3]);
    if(!Vehicle::exists($vehicleID))
    {
      header('HTTP/1.1 404 Not Found');
      $_GET['e'] = 404; include 'err.php';
      die();
    }
    $vehicle = Vehicle::getVehicle($vehicleID);
    $userID = $vehicle->owner;

    if(!($token = getBearerToken()))
    {
      header('HTTP/1.1 401 Unauthorized');
      $_GET['e'] = 401; include 'err.php';
      die();
    }

    $valid = Token::validate($token, $_ENV['JWT_SECRET']) && 
      Token::validateExpiration($token, $_ENV['JWT_SECRET']);

    $payload = Token::getPayload($token, $_ENV['JWT_SECRET']);

    if(!$valid || $payload['user_id'] != $userID)
    {
      header('HTTP/1.1 403 Forbidden');
      $_GET['e'] = 403; include 'err.php';
      die();
    }

    if(!isset($query[4]) || empty($query[4]))
    {
      // .../api/vehicles/*
      switch ($_SERVER['REQUEST_METHOD'])
      {
      case 'GET':
        //Return vehicle information
        header('HTTP/1.1 200 OK');
        echo json_encode($vehicle);
        break;
      case 'PUT':
        //Update vehicle information
        $data = json_decode(file_get_contents('php://input'));
        Vehicle::updateVehicle($vehicleID, $data->name, $data->color);
        header('HTTP/1.1 204 No Content');
        header('Content-Location: /api/vehicles/' . $vehicleID);
        break;
      case 'DELETE':
        //Delete vehicle
        Vehicle::deleteVehicle($vehicleID);
        header('HTTP/1.1 204 No Content');
        break;
      default:
        header("HTTP/1.1 405 Method not Allowed");
        $_GET['e'] = 405; include 'err.php';
      }
    }
    else
    {
      switch ($query[4])
      {
      case 'sensors':
        if(!isset($query[5]) || empty($query[5]))
        {
          // .../api/vehicles/*/sensors
          switch ($_SERVER['REQUEST_METHOD'])
          {
          case 'GET':
            //Return the list of sensors of this vehicle
            $sensor = Sensor::getSensors($vehicleID);
            header('HTTP/1.1 200 OK');
            echo json_encode($sensor);
            break;
          case 'POST':
            //Add a new sensor to this vehicle
            $data = json_decode(file_get_contents('php://input'));
            $newId = Sensor::createSensor($vehicleID, $data->name, 
              $data->data_type);
            header('HTTP/1.1 201 Created');
            header('Content-Location: /api/vehicles/' . $vehicleID . 
              '/sensors/' . $newId);
            break;
          default:
            header("HTTP/1.1 405 Method not Allowed");
            $_GET['e'] = 405; include 'err.php';
          }
        }
        else
        {
          $sensorID = intval($query[5]);
          if(!Sensor::exists($sensorID))
          {
            header('HTTP/1.1 404 Not Found');
            $_GET['e'] = 404; include 'err.php';
            die();
          }

          $sensor = Sensor::getSensor($sensorID);
          if($sensor->vehicle != $vehicleID)
          {
            header("HTTP/1.1 400 Bad Request");
            $_GET['e'] = 400; include 'err.php';
            die();
          }

          // .../api/vehicles/*/sensors/*
          switch ($_SERVER['REQUEST_METHOD'])
          {
          case 'GET':
            //Return information regarding the sensor
            header('HTTP/1.1 200 OK');
            echo json_encode($sensor);
            break;
          case 'PUT':
            //Update information about the sensor
            $data = json_decode(file_get_contents('php://input'));
            Sensor::updateSensor($sensorID, $data->name, $data->data_type);
            header('HTTP/1.1 204 No Content');
            header('Content-Location: /api/vehicles/' . $vehicleID . 
              '/sensors/' . $sensorID);
            break;
          case 'DELETE':
            //Delete sensor
            Sensor::deleteSensor($sensorID);
            header('HTTP/1.1 204 No Content');
            break;
          default:
            header("HTTP/1.1 405 Method not Allowed");
            $_GET['e'] = 405; include 'err.php';
          }
        }
        break;
      case 'measurements':
        if(!isset($query[5]) || empty($query[5]))
        {
          // .../api/vehicles/*/measurements
          switch ($_SERVER['REQUEST_METHOD'])
          {
          case 'GET':
            //Return list of measurements made on this vehicle
            if(isset($_GET['count']))
            {
              $count['count'] = Measurement::number($vehicleID);
              header('HTTP/1.1 200 OK');
              echo json_encode($count);
            }
            else
            {
              if(isset($_GET['limit']))
              {
                if(isset($_GET['offset']))
                {
                  $measurements = Measurement::getMeasurementsLimitOffset(
                    $vehicleID, $_GET['limit'], $_GET['offset']);
                }
                else
                {
                  $measurements = Measurement::getMeasurementsLimitOffset(
                    $vehicleID, $_GET['limit']);
                }
              }
              else
              {
                $measurements = Measurement::getMeasurements($vehicleID);
              }
              header('HTTP/1.1 200 OK');
              echo json_encode($measurements);
            }
            break;
          case 'POST':
            //Make a new measurement
            $data = json_decode(file_get_contents('php://input'));
            try{
              $newId = Measurement::createMeasurement($vehicleID,
                $data->sensors, $data->values);
              header('HTTP/1.1 201 Created');
              header("Content-Location: /api/vehicles/$vehicleID/measurements/"
                . $newId);
            }catch(Exception $e){
              header('HTTP/1.1 400 Bad Request');
              $_GET['e'] = 400; include 'err.php';
              echo "<p>$e->getMessage()</p>";
              die();
            }
            break;
          case 'DELETE':
            //Delete all measurements of the vehicle
            Measurement::deleteAllMeasurements($vehicleID);
            header('HTTP/1.1 204 No Content');
            break;
          default:
            header("HTTP/1.1 405 Method not Allowed");
            $_GET['e'] = 405; include 'err.php';
          }
        }
        else
        {
          $measurementID = intval($query[5]);
          if(!Measurement::exists($measurementID))
          {
            header('HTTP/1.1 404 Not Found');
            $_GET['e'] = 404; include 'err.php';
            die();
          }

          $measurement = Measurement::getMeasurement($measurementID);
          if($measurement->vehicle != $vehicleID)
          {
            header("HTTP/1.1 400 Bad Request");
            $_GET['e'] = 400; include 'err.php';
            die();
          }

          // .../api/vehicles/*/measurements/*
          switch ($_SERVER['REQUEST_METHOD'])
          {
          case 'GET':
            //Return measurement information
            header('HTTP/1.1 200 OK');
            echo json_encode($measurement);
            break;
          case 'DELETE':
            //Delete measurement
            Measurement::deleteMeasurement($measurementID);
            header('HTTP/1.1 204 No Content');
            break;
          default:
            header("HTTP/1.1 405 Method not Allowed");
            $_GET['e'] = 405; include 'err.php';
          }
        }
        break;
      default:
        header('HTTP/1.1 404 Not Found');
        $_GET['e'] = 404; include 'err.php';
      }
    }
  }
  break;
case 'login':
  switch ($_SERVER['REQUEST_METHOD'])
  {
  case 'POST':
    $data = json_decode(file_get_contents('php://input'));
    if($user = User::findUserByName($data->name))
    {
      if($user->verifyPassword($data->password))
      {
        $payload = [
          'user_id' => $user->id,
          'user_name' => $user->name,
          'iat' => time(),
          'exp' => time() + 3600,
          'iss' => 'localhost'
        ];

        $accessToken = Token::customPayload($payload, $_ENV['JWT_SECRET']);

        $token['accessToken'] = $accessToken;
        header('HTTP/1.1 200 OK');
        echo json_encode($token);
      }
      else
      {
        header('HTTP/1.1 400 Bad Request');
        $_GET['e'] = 400; include 'err.php';
      }
    }
    else
    {
      header('HTTP/1.1 400 Bad Request');
      $_GET['e'] = 400; include 'err.php';
    }
    break;
  default:
    header('HTTP/1.1 405 Method not Allowed');
    $_GET['e'] = 405; include 'err.php';
  }
  break;
default:
  header('HTTP/1.1 404 Not Found');
  $_GET['e'] = 404; include 'err.php';
}

function getAuthoriationHeader()
{
  $headers = null;
  if(function_exists('apache_request_headers'))
  {
    $requestHeaders = apache_request_headers();
    $requestHeaders = array_combine(array_map('ucwords',
      array_keys($requestHeaders)), array_values($requestHeaders));
    if(isset($requestHeaders['Authorization']))
    {
      $headers = trim($requestHeaders['Authorization']);
    }
  }
  return $headers;
}

function getBearerToken()
{
  $headers = getAuthoriationHeader();
  if(!empty($headers))
  {
    if(preg_match('/Bearer\s(\S+)/', $headers, $matches))
    {
      return $matches[1];
    }
  }
  return null;
}

?>
