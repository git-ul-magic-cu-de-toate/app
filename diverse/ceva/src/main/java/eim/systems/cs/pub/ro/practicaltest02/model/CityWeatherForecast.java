package eim.systems.cs.pub.ro.practicaltest02.model;

public class CityWeatherForecast {

    private String temperature;
    private String windSpeed;
    private String generalStatus;
    private String pressure;
    private String humidity;

    public CityWeatherForecast() {

    }

    public CityWeatherForecast(String temperature, String pressure, String humidity, String generalStatus, String windSpeed) {
        this.temperature = temperature;
        this.pressure = pressure;
        this.humidity = humidity;
        this.generalStatus = generalStatus;
        this.windSpeed = windSpeed;
    }

    public String getGeneralStatus() {
        return generalStatus;
    }

    public void setGeneralStatus(String generalStatus) {
        this.generalStatus = generalStatus;
    }

    public String getHumidity() {
        return humidity;
    }

    public void setHumidity(String humidity) {
        this.humidity = humidity;
    }

    public String getPressure() {
        return pressure;
    }

    public void setPressure(String pressure) {
        this.pressure = pressure;
    }

    public String getTemperature() {
        return temperature;
    }

    public void setTemperature(String temperature) {
        this.temperature = temperature;
    }

    public String getWindSpeed() {
        return windSpeed;
    }

    public void setWindSpeed(String windSpeed) {
        this.windSpeed = windSpeed;
    }
}
