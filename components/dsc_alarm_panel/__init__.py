import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

component_ns = cg.esphome_ns.namespace('alarm_panel')
Component = component_ns.class_('DSCkeybushome', cg.PollingComponent)

CONF_ACCESSCODE="accesscode"
CONF_MAXZONES="maxzones"
CONF_USERCODES="usercodes"
CONF_DEFAULTPARTITION="defaultpartition"
CONF_DEBUGLEVEL="dscdebuglevel"
CONF_READPIN="dscreadpin"
CONF_WRITEPIN="dscwritepin"
CONF_CLOCKPIN="dscclockpin"
CONF_EXPANDER1="expanderaddr1"
CONF_EXPANDER2="expanderaddr2"


systemstatus= '''[&](std::string statusCode) {
      alarm_panel::publishTextState("ss",0,&statusCode); 
    }'''
partitionstatus= '''[&](std::string statusCode,uint8_t partition) {
      alarm_panel::publishTextState("ps_",partition,&statusCode); 
      alarm_panel::publishBinaryState("al_",partition,(statusCode.compare("triggered")==0));        
    }'''    
partitionmsg= '''[&](std::string msg,uint8_t partition) {
      alarm_panel::publishTextState("msg_",partition,&msg); 
    }'''    
panelstatus= '''[&](alarm_panel::panelStatus ps,bool open,uint8_t partition) {
      std::string sensor="NIL";
        switch(ps) {
          case alarm_panel::trStatus: sensor = "tr"; break;
          case alarm_panel::batStatus: sensor = "bat"; break;
          case alarm_panel::acStatus: sensor = "ac"; break;
          case alarm_panel::rdyStatus:  sensor = "rdy_" ; break;
          case alarm_panel::armStatus:  sensor = "arm_"; break; 
          default: break;
        }                
      alarm_panel::publishBinaryState(sensor.c_str(),partition,open);
    }'''      
line1 ='''[&](std::string msg,uint8_t partition) {
      alarm_panel::publishTextState("ln1_",partition,&msg);
    }'''
line2='''[&](std::string msg,uint8_t partition) {
      alarm_panel::publishTextState("ln2_",partition,&msg);
    }'''
beeps='''[&](std::string  beeps,uint8_t partition) {
      alarm_panel::publishTextState("bp_",partition,&beeps); 
    }'''
zonemsg='''[&](std::string msg) {
      alarm_panel::publishTextState("zs",0,&msg);  
    }'''
troublemsg='''[&](std::string msg) {
      alarm_panel::publishTextState("tr_msg",0,&msg);  
    }'''    
eventinfo='''[&](std::string msg) {
      alarm_panel::publishTextState("evt",0,&msg);  
    }''' 
    
firestatus='''[&]( bool open,uint8_t partition) {
      alarm_panel::publishBinaryState("fa_",partition,open);    
    }'''
zonebinary='''[&](int zone, bool open) {
      std::string sensor = "z" + std::to_string(zone) ;
      alarm_panel::publishBinaryState(sensor.c_str(),0,open);    
    }'''
relay='''[&](uint8_t channel,bool open) {
      std::string sensor = "r"+ std::to_string(channel);
      alarm_panel::publishBinaryState(sensor.c_str(),0,open);       
    }'''

CONFIG_SCHEMA = cv.Schema(
    {
    cv.GenerateID(): cv.declare_id(Component),
    cv.Optional(CONF_ACCESSCODE, default=""): cv.string  ,
    cv.Optional(CONF_MAXZONES, default=""): cv.int_, 
    cv.Optional(CONF_USERCODES, default=""): cv.string, 
    cv.Optional(CONF_DEFAULTPARTITION, default=""): cv.int_, 
    cv.Optional(CONF_DEBUGLEVEL, default=""): cv.int_, 
    cv.Optional(CONF_READPIN, default=""): cv.int_, 
    cv.Optional(CONF_WRITEPIN, default=""): cv.int_, 
    cv.Optional(CONF_CLOCKPIN, default=""): cv.int_, 
    cv.Optional(CONF_EXPANDER1, default=""): cv.int_, 
    cv.Optional(CONF_EXPANDER2, default=""): cv.int_, 
    
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID],config[CONF_CLOCKPIN],config[CONF_READPIN],config[CONF_WRITEPIN])
    
    if CONF_ACCESSCODE in config:
        cg.add(var.set_accessCode(config[CONF_ACCESSCODE]));
    if CONF_MAXZONES in config:
        cg.add(var.set_maxZones(config[CONF_MAXZONES]));
    if CONF_USERCODES in config:
        cg.add(var.set_userCodes(config[CONF_USERCODES]));
    if CONF_DEFAULTPARTITION in config:
        cg.add(var.set_defaultPartition(config[CONF_DEFAULTPARTITION]));
    if CONF_DEBUGLEVEL in config:
        cg.add(var.set_debug(config[CONF_DEBUGLEVEL]));
    if CONF_EXPANDER1 in config:
        cg.add(var.set_expanderAddr(1,config[CONF_EXPANDER1]));
    if CONF_EXPANDER2 in config:
        cg.add(var.set_expanderAddr(2,config[CONF_EXPANDER2]));
       
    cg.add(var.onSystemStatusChange(cg.RawExpression(systemstatus)))  
    cg.add(var.onPartitionStatusChange(cg.RawExpression(partitionstatus)))  
    cg.add(var.onPartitionMsgChange(cg.RawExpression(partitionmsg)))  
    cg.add(var.onPanelStatusChange(cg.RawExpression(panelstatus)))  
    cg.add(var.onLine1Display(cg.RawExpression(line1))) 
    cg.add(var.onLine2Display(cg.RawExpression(line2)))    
    cg.add(var.onBeeps(cg.RawExpression(beeps)))    
    cg.add(var.onZoneMsgStatus(cg.RawExpression(zonemsg)))   
    cg.add(var.onEventInfo(cg.RawExpression(eventinfo)))    
    cg.add(var.onZoneStatusChange(cg.RawExpression(zonebinary)))    
    cg.add(var.onFireStatusChange(cg.RawExpression(firestatus)))   
    cg.add(var.onTroubleMsgStatus(cg.RawExpression(troublemsg)))    
    cg.add(var.onRelayChannelChange(cg.RawExpression(relay)))      
    await cg.register_component(var, config)
    