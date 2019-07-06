local API_ADDRESS = 'http://worldtimeapi.org/api/timezone/Europe/Warsaw'
local LED_PIN = 4
local timeData

function blink()
    gpio.write(LED_PIN, gpio.LOW)
    tmr.delay(100 * 1000)
    gpio.write(LED_PIN, gpio.HIGH)
end

function getTimeDate()
    local timeDateJson = ""
    http.get(API_ADDRESS, nil, function(code, data)
        if (code >= 0) then
            handleTimeDate(data)
        else
            print('RequestError')
        end
    end)

    return timeDateJson
end

function handleTimeDate(response)
    responseJson = sjson.decode(response)

    timeDateString = responseJson['datetime']
    year, month, day = string.match(timeDateString, '(%d+)-(%d+)-(%d+)')
    hour, min, sec = string.match(timeDateString, 'T(%d+):(%d+):(%d+)')
    weekday = responseJson['day_of_week']

    uart.write(0, string.format("%d:%d:%d/%d-%d-%d/%d\n", hour, min, sec, year, month, day, weekday))
end
