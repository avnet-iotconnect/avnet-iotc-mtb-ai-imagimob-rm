# /IOTCONNECT Gesture-Controlled Street View Demo Setup

This guide provides a detailed step-by-step overview to set up an AWS-based infrastructure that enables an Imagimob gesture model connected via /IOTCONNECT to remotely control a Google Street View-based website.

## Overview
This solution leverages /IOTCONNECT to send recognized gestures to AWS through webhooks, storing gesture data and locations in DynamoDB. A webpage hosted in AWS S3 polls the gesture data from DynamoDB via AWS API Gateway and Lambda, dynamically updating the Google Street View based on received gestures.

## AWS Services Block Diagram

```
+------------+          Webhook          +-----------------+
| /IOTCONNECT | ----------------------->  | AWS API Gateway |
+------------+                           +-----------------+
                                                   |
                                                   v
                                           +----------------+
                                           | AWS Lambda     |
                                           +----------------+
                                                   |
                                                   v
                                           +----------------+
                                           | Amazon DynamoDB|
                                           +----------------+
                                                   ^
                                                   |
+-------------------+     Polling via API Gateway |
| Webpage Hosted    | <----------------------------+
| on Amazon S3      |
| (Google StreetView|
| with JavaScript)  |
+-------------------+
```

## AWS Setup Steps

### 1. Create DynamoDB Table
- Navigate to AWS DynamoDB in the AWS console.
- Create a new table named `GestureTable` with a Partition key named `id` (String).

### 2. Create AWS Lambda Functions

Create two Lambda functions:

#### setGesture
- **Runtime:** Python 3.11
- **Execution role:** Must include DynamoDB `PutItem` permissions.
- **Function code:**

```python
import boto3

def lambda_handler(event, context):
    headers = {k.lower(): v for k, v in event.get('headers', {}).items()}
    gesture = headers.get('gesture', 'none')
    lat = headers.get('latitude', '33.0020')  # default example
    lng = headers.get('longitude', '-96.7670')

    dynamodb = boto3.resource('dynamodb')
    table = dynamodb.Table('GestureTable')
    table.put_item(Item={
        'id':'latest',
        'gesture': gesture,
        'lat': lat,
        'lng': lng
    })

    return {'statusCode': 200, 'body': 'OK'}
```

#### getGesture
- **Runtime:** Python 3.11
- **Execution role:** Must include DynamoDB `GetItem` and `PutItem` permissions.
- **Function code:**

```python
import json, boto3

def lambda_handler(event, context):
    dynamodb = boto3.resource('dynamodb')
    table = dynamodb.Table('GestureTable')
    response = table.get_item(Key={'id':'latest'})
    item = response.get('Item', {})
    gesture = item.get('gesture', 'none')
    lat = item.get('lat', '33.0020')
    lng = item.get('lng', '-96.7670')

    table.put_item(Item={'id':'latest', 'gesture':'none', 'lat': lat, 'lng': lng})

    return {
        'statusCode': 200,
        'body': json.dumps({'gesture': gesture, 'lat': lat, 'lng': lng}),
        'headers': {'Content-Type':'application/json','Access-Control-Allow-Origin':'*'}
    }
```

### 3. Setup AWS API Gateway (HTTP API)

- Create an HTTP API named `GestureAPI`.
- Create two routes:
  - `/prod/setgesture` (POST): Integrated with `setGesture` Lambda.
  - `/prod/getgesture` (GET): Integrated with `getGesture` Lambda.
- Deploy API and note the URL (e.g., `https://{api_id}.execute-api.region.amazonaws.com/prod`).

### 4. Host Webpage in AWS S3

- Create an S3 bucket named `gesture-streetview-demo`.
- Enable static website hosting (index.html).
- Upload the following HTML file after replacing the API URL and Google Maps API key:

```html
<!DOCTYPE html>
<html>
<head>
  <title>Gesture-Controlled Street View</title>
  <style>html,body{height:100%;margin:0;padding:0;}#street-view{height:100%;width:100%;}</style>
  <script src="https://maps.googleapis.com/maps/api/js?key=YOUR_GOOGLE_API_KEY"></script>
</head>
<body>
  <div id="street-view"></div>
  <script>
    let panorama, position, pov = {heading:0,pitch:0};

    async function initStreetView(){
      let res=await fetch('YOUR_API_GATEWAY_URL/getgesture');
      let data=await res.json();
      position={lat:parseFloat(data.lat),lng:parseFloat(data.lng)};
      panorama=new google.maps.StreetViewPanorama(document.getElementById("street-view"),{position,pov,zoom:1});
      setInterval(getGesture,1000);
    }

    async function getGesture(){
      let res=await fetch('YOUR_API_GATEWAY_URL/getgesture');
      let data=await res.json();
      if(data.gesture!=="none"){
        switch(data.gesture){
          case"up":pov.pitch-=10;break;
          case"down":pov.pitch+=10;break;
          case"left":pov.heading-=20;break;
          case"right":pov.heading+=20;break;
          case"push":
            let headingRad=(pov.heading*Math.PI)/180;
            position.lat+=0.0001*Math.cos(headingRad);
            position.lng+=0.0001*Math.sin(headingRad);
            panorama.setPosition(position);
            break;
        }
        panorama.setPov(pov);
      }
    }

    initStreetView();
  </script>
</body>
</html>
```

- Update bucket policy to allow public read.

## /IOTCONNECT Webhook Setup

- Webhook URL: Your AWS API Gateway URL (`https://{api_id}.execute-api.region.amazonaws.com/prod/setgesture`).
- Custom Headers Example:

| Key | Value |
|-----|-------|
|gesture|left|
|latitude|33.0020|
|longitude|-96.7670|

Create multiple webhook rules in /IOTCONNECT with appropriate gestures and locations as needed.

## Testing the Setup

Trigger gestures via /IOTCONNECT, and your S3-hosted webpage will respond dynamically, demonstrating a fully integrated IoT and cloud-powered gesture-controlled virtual experience.

