from enum import Enum

class GPSDReport:
    @staticmethod
    def getReport(jsonReport):
        reportType = jsonReport.get('class')
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
        else:
            return None


class TPVReport(GPSDReport):
    class ModeEnum(Enum):
        NO_MODE = 0
        NO_FIX = 1
        DIM_2 = 2
        DIM_3 = 3

    def __init__(self, jsonReport):
        self.mode = jsonReport.get('mode')
        self.time = jsonReport.get('time')
        self.timeErrSec = jsonReport.get('ept')# time error in seconds
        self.lonDeg = jsonReport.get('lat')# latitude, degrees
        self.latDeg = jsonReport.get('lon')# longitude, degrees
        self.altMeters = jsonReport.get('alt')# altitude, meters
        self.lonErrMeters = jsonReport.get('epx')# longitude error, meters
        self.latErrMeters = jsonReport.get('epy')# latitude error, meters
        self.altErrMeters = jsonReport.get('epv')# altitude error, meters
        self.headingDeg = jsonReport.get('track')# degrees from true north
        self.speedMPS = jsonReport.get('speed')# speed in meters per second
        self.climbMPS = jsonReport.get('climb')# climb/sink rate in meters per second
        self.headingErrDeg = jsonReport.get('epd')# heading error in degrees
        self.speedErrMPS = jsonReport.get('eps')# speed error in meters per second
        self.climbErrMPS = jsonReport.get('epc')# climb/sink error in meters per second
    
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