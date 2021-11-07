class GPSDReport:
    @staticmethod
    def getReport(jsonReport):
        reportType = jsonReport.get('class', None)
        if reportType is None:
            return None
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
        self.time = jsonReport.get('time', None)  
        self.timeErrSec = jsonReport.get('ept', None)  # time error in seconds
        self.lonDeg = jsonReport.get('lat', None)  # latitude, degrees
        self.latDeg = jsonReport.get('lon', None)  # longitude, degrees
        self.altMeters = jsonReport.get('alt', None)  # altitude, meters
        self.lonErrMeters = jsonReport.get('epx', None)  # longitude error, meters
        self.latErrMeters = jsonReport.get('epy', None)  # latitude error, meters
        self.altErrMeters = jsonReport.get('epv', None)  # altitude error, meters
        self.headingDeg = jsonReport.get('track', None)  # degrees from true north
        self.speedMPS = jsonReport.get('speed', None)  # speed in meters per second
        self.climbMPS = jsonReport.get('climb', None)  # climb/sink rate in meters per second
    
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