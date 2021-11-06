class GPSDReport:
    @staticmethod
    def getReport(jsonReport):
        reportType = jsonReport['class']
        if reportType == 'TPV':# Time Position Velocity Reports
            return TPVReport(jsonReport)
        elif reportType == 'VERSION':# Report about the gpsd version
            return VersionReport(jsonReport)
        elif reportType == 'SKY':# Sky Reports about the constellation
            return SkyReport(jsonReport)
        elif reportType == 'DEVICES':# Report about connected devices
            return DevicesReport(jsonReport)
        elif reportType == 'DEVICE':# Report about an individual device
            return DeviceReport(jsonReport)
        elif reportType == 'WATCH':# Response to watch command
            return WatchReport(jsonReport)
        elif reportType == 'ERROR':# Error Reports
            return ErrorReport(jsonReport)


class TPVReport(GPSDReport):    
    def __init__(self, jsonReport):
        self.time = jsonReport['time']
        self.timeErrSec = jsonReport['ept']# time error in seconds
        self.lonDeg = jsonReport['lat']# latitude, degrees
        self.latDeg = jsonReport['lon']# longitude, degrees
        self.altMeters = jsonReport['alt']# altitude, meters
        self.lonErrMeters = jsonReport['epx']# longitude error, meters
        self.latErrMeters = jsonReport['epy']# latitude error, meters
        self.altErrMeters = jsonReport['epv']# altitude error, meters
        self.headingDeg = jsonReport['track']# degrees from true north
        self.speedMPS = jsonReport['speed']# speed in meters per second
        self.climbMPS = jsonReport['climb']# climb/sink rate in meters per second
    
class VersionReport(GPSDReport):
    def __init__(self, jsonReport):
        pass

class SkyReport(GPSDReport):
    def __init__(self, jsonReport):
        pass

class DevicesReport(GPSDReport):
    def __init__(self, jsonReport):
        pass

class DeviceReport(GPSDReport):
    def __init__(self, jsonReport):
        pass

class WatchReport(GPSDReport):
    def __init__(self, jsonReport):
        pass

class ErrorReport(GPSDReport):
    def __init__(self, jsonReport):
        pass