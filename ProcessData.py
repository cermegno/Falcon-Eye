import datetime
import time
import twitter

#This is my first Python program ever. Hopefully you don't find it offensive :)

TempThreshold = 27
HumiThreshold = 40
LighThreshold = 1000
AlfaAlert = False
PreviousAlert = False
BaseFillGreen = '#55DD55' #These are color definitions
BaseFillRed = '#FF5555'
B1color = BaseFillGreen #We assume everything is right till proven wrong


consumer_key='aaaaaaaaaaaaaaaaaaaaa'
consumer_secret='bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb'
access_token_key='cccccccccccccccccccccccccccccccccccccccccc'
access_token_secret='dddddddddddddddddddddddddddddddddddddddddddd'

api = twitter.Api(consumer_key, consumer_secret,
                  access_token_key, access_token_secret)

while True:

    #Open the vehicles log file
    v = open("vehicles.log", 'r')
    data = v.readlines()    #Read the file into a list
    v.close()

    #Create a web page with vehicles log historical data
    g = open("html/vehicles.html", 'w')
    g.write("<body background=\"marsbgr.jpg\"><center>")
    g.write("<table border=\"1\"style=\"color:#D28432\">")
    g.write("<tr><td><b>VEHICLE ACCESS EVENTS</b>")
    for line in data:
        d = "<tr><td>"+line+"\n"
        g.write(d)
    g.close()

    #Open the log file, get the last line and read its values
    #In a multibase scenario each base will have its log file
    #I should use then AlfaLastReading, BetaLastReading ...
    f = open("readings.log", 'r')
    data = f.readlines()    #Read the file into a list
    f.close()
    lastReading = data[len(data)-1]
    print lastReading

    #Create a file with the last reading for the table in mobile app
    f = open("html/lastreadings.log", 'w')
    f.write(lastReading)
    f.close()

    #Create a file with clean readings for mobile app without base or epoch
    #Create a web page with historical data
    f = open("html/cleanreadings.log", 'w')
    g = open("html/historical.html", 'w')
    g.write("<body background=\"marsbgr.jpg\"><center>")
    g.write("<table border=\"1\"style=\"color:#D28432\">")
    g.write("<tr><td>TEMP C<td>HUMIDITY%<td>RADIATION<td>TIME")
    for line in data:
        Fields = line.split(',', 6 ) #Split in 5 chunks. Separate on commas
        c = str(Fields[1])+"   "+str(Fields[2])+"  "+str(Fields[3])+" "+str(Fields[5])
        f.write(c)
        d = "<tr><td>"+str(Fields[1])+"<td>"+str(Fields[2])+"<td>"+str(Fields[3])+"<td>"+str(Fields[5])+"\n"
        g.write(d)
    f.close()
    g.close()

    #Need to extract last values to change colours in the webpage
    lastValues = lastReading.split(',', 5 ) #Split in 5 chunks. Separate on commas
    t = int (lastValues[1])
    h = int (lastValues[2])
    l = int (lastValues[3])
    ts = str (lastValues[5])
    
    # Now compare against the thresholds we defined above
    if (t > TempThreshold or
        h > HumiThreshold or
        l > LighThreshold):
        AlfaAlert = True
    else:
        AlfaAlert = False

    print 'New Alert is ',AlfaAlert
    print 'Previous Alert was ',PreviousAlert
    status = ""
    if AlfaAlert == True:
        B1color = BaseFillRed   #Need to change the color in the map anyway
        if PreviousAlert == False:
            print 'Alfa is gone bad, we need to notify'
            #Twitt section
#            status = api.PostUpdate('Alert in base Alfa, temp='+str(t)+" hum="+str(h)+" light="+str(l)+" time="+ts)
#            print status.text
            
    if AlfaAlert == False:
        B1color = BaseFillGreen   #Need to change the color in the map anyway
        if PreviousAlert == True:
            print 'Alfa is healthy now, we need to notify'
            #Twitt section
#            status = api.PostUpdate('Base Alfa back to normal,temp='+str(t)+" hum="+str(h)+" light="+str(l)+" time="+ts)
#            print status.text

    PreviousAlert = AlfaAlert    

    #preparing the time section
    now = str(datetime.datetime.fromtimestamp(float(lastValues[4])))
    TimeSection = 'Showing readings from ' + now

    #Now prepare the map section
    #In this dictionary Base2 and Base3 have static green color for now
    dict1 = {'Base1Fill':    B1color,
            'Base2Fill':    BaseFillGreen,
            'Base3Fill':    BaseFillGreen}

    #Add extra items in the future when Beta and Gamma are online
    dict2 = {'HdrCol':   '#9BBB59',
            'Row1Col':   '#CDDDAC',
            'Row2Col':   '#E6EED5',
            'AlfaTemp':  t,
            'AlfaHum':   h,
            'AlfaLight': l}

    MapSection = '''
    <!DOCTYPE HTML>
    <html><head>
    <meta charset="utf-8">
    <meta http-equiv="refresh" content="30" />
      <title>Mars Colony Status</title>
      <style type="text/css" media="screen">
        canvas, img {{ display:block; margin:1em auto; border:1px solid black; }}
        canvas {{ background:url(marsFull.png) }}
      </style>
    </head>

    <body style="font-family:'Century Gothic';background-color:#FAFAC6">
    <canvas width="1194" height="516"></canvas>

    <script type="text/javascript" charset="utf-8">
      var can = document.getElementsByTagName('canvas')[0];
      var ctx = can.getContext('2d');

      ctx.strokeStyle = '#777722';
      ctx.lineWidth   = 2;
      ctx.lineJoin    = 'round';
      ctx.strokeRect(380,170,100,65);
      ctx.shadowBlur=30;
      ctx.shadowColor="black";
      ctx.fillStyle = '{Base1Fill}';
      ctx.fillRect(381,171,98,63);
      ctx.strokeStyle = '#110000';
      ctx.shadowBlur=0;
      ctx.fillStyle = "#111111";
      ctx.font="30px 'Century gothic'";
      ctx.fillText("Alfa",400,210);

      ctx.strokeStyle = '#777722';
      ctx.lineWidth   = 2;
      ctx.lineJoin    = 'round';
      ctx.strokeRect(720,100,80,55);
      ctx.shadowBlur=30;
      ctx.shadowColor="black";
      ctx.fillStyle = '{Base2Fill}';
      ctx.fillRect(721,101,78,53);
      ctx.shadowBlur=0;
      ctx.fillStyle = "#111111";
      ctx.font="25px 'Century gothic'";
      ctx.fillText("Beta",732,135);

      ctx.strokeStyle = '#777722';
      ctx.lineWidth   = 2;
      ctx.lineJoin    = 'round';
      ctx.strokeRect(800,400,140,65);
      ctx.shadowBlur=30;
      ctx.shadowColor="black";
      ctx.fillStyle = '{Base3Fill}';
      ctx.fillRect(801,401,138,63);
      ctx.shadowBlur=0;
      ctx.fillStyle = "#111111";
      ctx.font="30px 'Century gothic'";
      ctx.fillText("Gamma",808,440);
    </script>
    <center>
    '''.format(**dict1)

    #Now prepare the table section
    TableSection = '''
    <table  style="width:430px" background-color=white border=0 cellspacing=4 cellpadding=8>
    <tr style='"height:45px";background:{HdrCol}'>
      <td> <b>Base<b/>
      <td> <b>Temperature<b/>
      <td> <b>Humidity<b/>
      <td> <b>Radiation<b/>
    <tr style='"height:45px";background:{Row1Col}'>
      <td> <b>Alfa<b/>
      <td> {AlfaTemp}
      <td> {AlfaHum}
      <td> {AlfaLight}
    <tr style='"height:45px";background:{Row2Col}'>
      <td> <b>Beta<b/>
      <td> 25
      <td> 63
      <td> 554
    <tr style='"height:45px";background:{Row1Col}'>
      <td> <b>Gamma<b/>
      <td> 23
      <td> 65
      <td> 550
    </table>
    '''.format(**dict2)

    # Assemble the Index web page
    f = open('html/index.html','w')
    f.write(MapSection)
    f.write(TimeSection)
    f.write(TableSection)
    f.write("<a href=\"historical.html\">Access Historical Data</a>")
    f.write("  |  ")
    f.write("<a href=\"vehicles.html\">Vehicle Access Events</a>")
    f.close()

    # Go to sleep
    time.sleep(10)

    
