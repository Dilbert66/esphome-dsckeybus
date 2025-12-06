console.info("%c  Alarm-keypad-card-dsc \n%c Version 0.1.5 ", "color: orange; font-weight: bold; background: black", "color: white; font-weight: bold; background: dimgray");
import { LitElement, html, css } from "https://unpkg.com/lit-element@4.0.0/lit-element.js?module";
import icons from './dsc_keypad_icons.js?id=015';

class AlarmKeypadCard extends LitElement {

  _line1=""
  _line2=""
  _beeps=""
  
  _readyStyle="color: var(--sensoroff);";
  _armedStyle="color: var(--sensoroff);";
  _chimeStyle="color: var(--sensoroff);";
  _troubleStyle="color: var(--sensoroff);";
  _acStyle="color: var(--sensoroff);";
  _dscKeypad=false;
  _scheme="";
  _rendered=false;
  

  render() {
    this._rendered=true;
    return html`
      <ha-card  header="${this._title}" color-scheme="${this._scheme}" style="--scale: ${this._scale};" >
<div class="flex-container">
  <div class="container"  >
          
      <div id="lcd_container">
        <div class="virtual_lcd">
          <div id="first_line">${this._line1}</div>
          <div id="second_line">${this._line2}</div>
         <div  id="beep_line">${this._beeps}</div>
        </div>
        <div class="status_icons">
          <i class="keypad-icon icon-check" id="ready_icon" title="Ready" style="${this._readyStyle}"></i>
          <i class="keypad-icon icon-armed" id="armed_icon" title="Armed" style="${this._armedStyle}"></i>
          ${this._sensor_chime?html`
          <i class="keypad-icon icon-bell" id="chime_icon" title="Chime" style="${this._chimeStyle}"></i>  
          `:''}        
          <i class="keypad-icon icon-trouble" id="trouble_icon" title="System Trouble" style="${this._troubleStyle}"></i>
          <i class="keypad-icon icon-ac" id="ac_icon" title="AC Power" style="${this._acStyle}"></i>
        </div>
      </div> <!-- lcd -->


      <div id="buttons_area">

        <div id="left_buttons">
 
          <div class="keypad_button_row">
            <button type="button" id="btn_<" class="btn btn-outline-dark keypad_button keypad_button_small" state="<"  @click="${this.setState}" title="<" >&lt;</button>
            <button type="button" id="btn_>" class="btn btn-outline-dark keypad_button keypad_button_small" state=">"  @click="${this.setState}" title=">" >&gt;</i></button>
          </div>
          <div class="keypad_button_row">
            <button type="button" id="btn_f" class="btn btn-outline-dark keypad_button keypad_button_slim" state="f"  @click="${this.confirmState}" title="Fire">
          <i class="keypad-icon icon-flame" ></i>
          </button>
          </div>
          <div class="keypad_button_row">
            <button type="button" id="btn_a" class="btn btn-outline-dark keypad_button keypad_button_slim" state="a"  @click="${this.confirmState}" title="Alert">
            <i class="keypad-icon icon-alert" ></i>
          </button>
          </div>
          <div class="keypad_button_row">
            <button type="button" id="btn_p" class="btn btn-outline-dark keypad_button keypad_button_slim" state="p"  @click="${this.confirmState}" title="Panic">
            <i class="keypad-icon icon-thief" ></i>
          </button>
           </div>
       </div> <!-- left buttons -->

        <div id="keypad_container">
          <div class="keypad_button_row">
            <button type="button" id="btn_1" class="btn btn-outline-dark keypad_button keypad_button_pad" state="1"  @click="${this.setState}" title="1">1<div class="keypad_cmd_text">${this._text_1}</div></button>
            <button type="button" id="btn_2" class="btn btn-outline-dark keypad_button keypad_button_pad" state="2"  @click="${this.setState}" title="2">2<div class="keypad_cmd_text">${this._text_2}</div></button>
            <button type="button" id="btn_3" class="btn btn-outline-dark keypad_button keypad_button_pad" state="3"  @click="${this.setState}" title="3">3<div class="keypad_cmd_text">${this._text_3}</div></button>
          </div>
          <div class="keypad_button_row">
            <button type="button" id="btn_4" class="btn btn-outline-dark keypad_button keypad_button_pad" state="4"  @click="${this.setState}" title="4">4<div class="keypad_cmd_text">${this._text_4}</div></button>
            <button type="button" id="btn_5" class="btn btn-outline-dark keypad_button keypad_button_pad" state="5"  @click="${this.setState}" title="5">5<div class="keypad_cmd_text">${this._text_5}</div></button>
            <button type="button" id="btn_6" class="btn btn-outline-dark keypad_button keypad_button_pad" state="6"  @click="${this.setState}" title="6">6<div class="keypad_cmd_text">${this._text_6}</div></button>
          </div>
          <div class="keypad_button_row">
            <button type="button" id="btn_7" class="btn btn-outline-dark keypad_button keypad_button_pad" state="7"  @click="${this.setState}" title="7">7<div class="keypad_cmd_text">${this._text_7}</div></button>
            <button type="button" id="btn_8" class="btn btn-outline-dark keypad_button keypad_button_pad" state="8"  @click="${this.setState}" title="8">8<div class="keypad_cmd_text">${this._text_8}</div></button>
            <button type="button" id="btn_9" class="btn btn-outline-dark keypad_button keypad_button_pad" state="9"  @click="${this.setState}" title="9">9<div class="keypad_cmd_text">${this._text_9}</div></button>
          </div>
          <div class="keypad_button_row">
            <button type="button" id="btn_*" class="btn btn-outline-dark keypad_button keypad_button_pad" state="*"  @click="${this.setState}" title="*"><i class="keypad-icon icon-star"></i><div class="keypad_cmd_text">${this._text_star}</div></button>
            <button type="button" id="btn_0" class="btn btn-outline-dark keypad_button keypad_button_pad" state="0"  @click="${this.setState}" title="0">0<div class="keypad_cmd_text">${this._text_0}</div></button>
            <button type="button" id="btn_#" class="btn btn-outline-dark keypad_button keypad_button_pad"  state="#"  @click="${this.setState}" title="#">#<div class="keypad_cmd_text">${this._text_pound}</div></button>
          </div>
        </div> <!-- keypad -->


        <div id="right_buttons">
 
          <div class="keypad_button_row">
            <button type="button" id="btn_s" class="btn btn-outline-dark keypad_button keypad_button_control" state="s"  @click="${this.setState}" title="stay">
            <i class="keypad-icon icon-stay_away"></i>
          </button>
          </div>
          <div class="keypad_button_row">
            <button type="button" id="btn_w" class="btn btn-outline-dark keypad_button keypad_button_control" state="w"  @click="${this.setState}" title="away">
            <i class="keypad-icon icon-stay_empty"></i>
          </button>
          </div>
          <div class="keypad_button_row">
            <button type="button" id="btn_c" class="btn btn-outline-dark keypad_button keypad_button_control" state="c"  @click="${this.setState}" title="chime">
          <i class="keypad-icon icon-bell"></i>
          </button>
          </div>
          <div class="keypad_button_row">
            <button type="button" id="btn_r" class="btn btn-outline-dark keypad_button keypad_button_control" state="r"  @click="${this.setState}" title="reset">
          <i class="keypad-icon icon-refresh"></i>
          </button>
          </div>
          <div class="keypad_button_row">
            <button type="button" id="btn_x" class="btn btn-outline-dark keypad_button keypad_button_control" state="x"  @click="${this.setState}" title="exit">
            <i class="keypad-icon icon-exit"></i>
          </button>
          </div>
        </div> <!-- right -->

     </div> <!-- buttons -->
    </div> <!-- container -->
</div> <!-- flex_container -->
    </ha-card>
                <audio id="exitsound1" loop>
                  <source src="/local/1_beep.mp3" type="audio/mpeg">
                </audio>
                <audio id="exitsound2" loop>
                  <source src="/local/2_beeps.mp3" type="audio/mpeg">
                </audio>
                <audio id="chime">
                  <source src="/local/3_beeps.mp3" type="audio/mpeg">
                </audio>
    `;

  }
  
 
  static get properties() {
    return {
        _config: Object,
        _title: String,
        _kpdline1: {
          type: Object,
        },
        _kpdline2: {
          type: Object,
        },
        _kpdbeep: {
          type: Object,            
        },
        _kpdsetting: {
          type: Object,            
        },
        



    };
  }


  setConfig(config) {
      if (!config.disp_line1) throw new Error('You need to define disp_line1');
      if (!config.disp_line2) throw new Error('You need to define disp_line2');
      if (!config.service_type) throw new Error('You need to define service_type');
      if (!config.service) throw new Error('You need to define service');
//      if (!config.sensor_ready) throw new Error('You need to define sensor_ready');
//      if (!config.sensor_armed) throw new Error('You need to define sensor_armed');
//      if (!config.sensor_trouble) throw new Error('You need to define sensor_trouble');
//      if (!config.sensor_ac) throw new Error('You need to define sensor_ac');

      this._config=config;
      this._title=config.title;
      this._kpdservicetype= config.service_type;
      this._kpdservice= config.service;
      this._scale=config.scale!=null?config.scale:"1";   
      this._cmd_stay=config.cmd_stay!=null?config.cmd_stay:"S";
      this._cmd_away=config.cmd_away!=null?config.cmd_away:"W"; 
      this._cmd_chime=config.cmd_chime!=null?config.cmd_chime:"C";
      this._cmd_reset=config.cmd_reset!=null?config.cmd_reset:"R";  
      this._cmd_exit=config.cmd_exit!=null?config.cmd_exit:"X"; 
      this._cmd_fire=config.cmd_fire!=null?config.cmd_fire:"F"; 
      this._cmd_alert=config.cmd_alert!=null?config.cmd_alert:"A"; 
      this._cmd_panic=config.cmd_panic!=null?config.cmd_panic:"P";      
      this._key_left=(config.key_left != null)?config.key_left:"<";
      this._key_right=(config.key_right != null)?config.key_right:">";
      this._partition=(config.partition != null)?config.partition:""; 
      this._key_0=(config.key_0 != null)?config.key_0:"0";
      this._key_1=(config.key_1 != null)?config.key_1:"1";
      this._key_2=(config.key_2 != null)?config.key_2:"2";
      this._key_3=(config.key_3 != null)?config.key_3:"3";
      this._key_4=(config.key_4 != null)?config.key_4:"4";
      this._key_5=(config.key_5 != null)?config.key_5:"5";
      this._key_6=(config.key_6 != null)?config.key_6:"6";
      this._key_7=(config.key_7 != null)?config.key_7:"7";
      this._key_8=(config.key_8 != null)?config.key_8:"8";
      this._key_9=(config.key_9 != null)?config.key_9:"9"; 
      this._key_star=(config.key_star != null)?config.key_star:"*";
      this._key_pound=(config.key_pound != null)?config.key_pound:"#";
      this._text_0=(config.text_0 != null)?config.text_0:"&nbsp;";
      this._text_1=(config.text_1 != null)?config.text_1:"BYPASS";
      this._text_2=(config.text_2 != null)?config.text_2:"SERV";
      this._text_3=(config.text_3 != null)?config.text_3:"ALARMS";
      this._text_4=(config.text_4 != null)?config.text_4:"CHIME";
      this._text_5=(config.text_5 != null)?config.text_5:"CODES";
      this._text_6=(config.text_6 != null)?config.text_6:"FUNC";
      this._text_7=(config.text_7 != null)?config.text_7:"OUTP";
      this._text_8=(config.text_8 != null)?config.text_8:"PROG";
      this._text_9=(config.text_9 != null)?config.text_9:"NIGHT";         
      this._text_star=(config.text_star != null)?config.text_star:"SELECT";
      this._text_pound=(config.text_pound != null)?config.text_pound:"ENTER";
      this._vibrate=(config.vibration_duration != null)?config.vibration_duration:5;

       this._style = config.style != null ? config.style : "";
       for (let i in this._style) {
        if (this._style[i] == null) continue;
        var v = this._style[i].replace(/;/gi, '');
        // console.log(i,v);
        this.style.setProperty(i, v);
       }
      if (this._hass) {
        this.hass = this._hass;
      }
   }


  displayChanged() {
    let state1 = "";
    let state2 = "";
    if (this._kpdline1  != undefined)
        for (let i = 0; i < this._kpdline1.state.length; i++) state1 += this._translateChar(this._kpdline1.state[i]);
    if (this._kpdline2 != undefined)
        for (let i = 0; i < this._kpdline2.state.length; i++) state2 += this._translateChar(this._kpdline2.state[i]);
    this._line1=state1;
    this._line2=state2;
  }



    beepChanged() {

        if (this._kpdbeep == undefined || !this._rendered) return;

        if (this._kpdbeep.state != null && this._kpdbeep != "0") {
            this._beeps="*** " + this._kpdbeep.state + " beep(s) ***";
        } 
        if (this._kpdbeep.state == "0" || this._kpdbeep.state == null) {
            this._beeps="";
            var promise = this.shadowRoot.getElementById("exitsound1").pause();
            this.shadowRoot.getElementById("exitsound2").pause();
            this.shadowRoot.getElementById("chime").pause();
        } else if (this._kpdbeep.state == "1") {
            var promise = this.shadowRoot.getElementById("exitsound1").play();
        } else if (this._kpdbeep.state == "2") {
            var promise = this.shadowRoot.getElementById("exitsound2").play();
        } else if (this._kpdbeep.state > 2) {
            var promise = this.shadowRoot.getElementById("chime").play();
        }

        if (promise !== undefined) {
            promise.then(_ => {
                // Autoplay started!
            }).catch(error => {
                console.warn('Sound auto play not enabled, check browser settings');
            });
        }
    }

  set hass(hass) {
    this._hass = hass;
    this._scheme=hass.themes.darkMode?"dark":"light";
    this._kpdline1 = this._hass.states[this._config.disp_line1];
    this._kpdline2 = this._hass.states[this._config.disp_line2];
    this._kpdbeep = this._hass.states[this._config.beep];
    this._sensor_ready=this._hass.states[this._config.sensor_ready]; 
    this._sensor_armed = this._hass.states[this._config.sensor_armed];    
    this._sensor_chime = this._hass.states[this._config.sensor_chime]; 
    this._sensor_trouble = this._hass.states[this._config.sensor_trouble]; 
    this._sensor_ac = this._hass.states[this._config.sensor_ac]; 
    this._readyStyle=(this._sensor_ready && this._sensor_ready.state=="on")?"color: green;":"color: var(--sensoroff);";
    this._acStyle=(this._sensor_ac && this._sensor_ac.state=="on")?"color: green;":"color: var(--sensoroff);";
    this._troubleStyle=(this._sensor_trouble && this._sensor_trouble.state=="on")?"color: orange;":"color: var(--sensoroff);";
    this._armedStyle=(this._sensor_armed && this._sensor_armed.state=="on")?"color: red;":"color: var(--sensoroff);";
    this._chimeStyle=(this._sensor_chime && this._sensor_chime.state=="on")?"color: green;":"color: var(--sensoroff);";
this.displayChanged();
this.beepChanged() 

}

  _translateChar(c) {
    // if (c.match('à') !== null ) return '<span class="blink">' + c + '</span>';
    // if (c.match('á') !== null ) return '<span class="under">' + c + '</span>';
    if (c.match('è') !== null ) return '░';
    if (c.match('é') !== null ) return '▓';

    return c;
  }

  stopPropagation(e) {
      e.stopPropagation();
  }


confirmState(e)   {
var key=e.currentTarget.getAttribute('state');
let msg="";
switch (key) {
    case 'a':msg="Alert";break;
    case 'f':msg="Fire";break;
    case 'p':msg="Panic";break;
    default:break;
}
if (confirm("Are you sure you want to trigger the "+msg + " command" ) == true)
    this.setState(e);
}

_isObject(obj) {
  return obj === Object(obj);
}

  setState(e) {
  
     var key=e.currentTarget.getAttribute('state');
      if (key==null|| key == "") return;
     switch (key) {
         case 's': key=this._cmd_stay; break;
         case 'w': key=this._cmd_away; break;
         case 'r': key=this._cmd_reset; break;
         case 'x': key=this._cmd_exit; break;  
         case 'c': key=this._cmd_chime; break;  
         case 'a': key=this._cmd_alert; break;  
         case 'f': key=this._cmd_fire; break; 
         case 'p': key=this._cmd_panic; break; 

        case '0': key = this._key_0; break;
        case '1': key = this._key_1; break;
        case '2': key = this._key_2; break;
        case '3': key = this._key_3; break;
        case '4': key = this._key_4; break;
        case '5': key = this._key_5; break;
        case '6': key = this._key_6; break;
        case '7': key = this._key_7; break;
        case '8': key = this._key_8; break;
        case '9': key = this._key_9; break;
        case '*': key = this._key_star; break;
        case '#': key = this._key_pound; break;
        case '>': key=this._key_right;break;
        case '<': key=this._key_left;break;
        default: return;
         
     }
     if (key == "") return;
        if ('vibrate' in navigator) {
            navigator.vibrate(this._vibrate);
        }
     if (!this._isObject(key)) {
        if (this._partition != "")
            key={'keys':key,'partition':this._partition};
        else
            key={'keys':key};
     }
     if (this._kpdservicetype.toLowerCase()=="mqtt") {
        let data={'topic':this._kpdservice,'payload':JSON.stringify(key)};
        this._hass.callService(this._kpdservicetype,"publish",data);
        
     } else if (this._kpdservicetype.toLowerCase()=="esphome") {  
        this._hass.callService(this._kpdservicetype, this._kpdservice,key);
     }
     //  this._hass.callService(this._kpdservicetype, this._kpdservice, key);
  }

confirmState(e)   {
var key=e.currentTarget.getAttribute('state');
if (confirm("Are you sure you want to send the cmd [" + key + "]?" ) == true)
    setState(e);
//else 
//    alert("Cmd ["+key+"] cancelled");
}

  getCardSize() {
      let size = 2;
     // if (this._config.view_pad) size += 4;     // 550px - 190px / 50
      return size;
  }

  applyStylesToRoot() {
      const sheet = icons.styleSheet;
      if (!document.adoptedStyleSheets.includes(sheet)) {
        document.adoptedStyleSheets.push(sheet);
      }
  }

  createRenderRoot() {
    this.applyStylesToRoot();
    return super.createRenderRoot();
  }

static getStubConfig() {

    return {

title: "DSC Alarm",
partition: "1",
service_type: "esphome",
service: "dscalarm_alarm_keypress_partition",
disp_line1: "sensor.dscalarm_line1_partition_1_ln1",
disp_line2: "sensor.dscalarm_line2_partition_1_ln2",
sensor_ready: "binary_sensor.dscalarm_partition_1_ready_rdy_1",
sensor_armed: "binary_sensor.dscalarm_partition_1_armed_arm_1",
sensor_trouble: "binary_sensor.dscalarm_trouble_status_tr",
sensor_ac: "binary_sensor.dscalarm_ac_status_ac",
beep: "sensor.dscalarm_beeps_bp_1",
text_1: "BYPASS",
text_2: "SERV",
text_3: "ALARMS",
text_4: "CHIME",
text_5: "CODES",
text_6: "FUNC",
text_7: "OUTP",
text_8: "PROG",
text_9: "NIGHT",
text_star: "SELECT",
text_pound: "ENTER",
vibration_duration: 5,
style: {
    "--buttonbgcolordark": "#252525",
    "--buttontextcolordark": "#03a9f4",
    "--lcdbgcolordark": "#859c99",
    "--lcdtextcolordark": "#000",
    "--sensoroffdark": "#303030",
    "--bordercolordark": "#333",
    "--hovercolordark": "#444",
    "--activecolordark": "#555",
    "--focuscolordark": "#454545",
    "--btngrpbgcolorlight":  "whitesmoke",
    "--buttonbgcolorlight": "#d9dcdf",
    "--buttontextcolorlight": "#000",
    "--lcdbgcolorlight": "#859c99",
    "--lcdtextcolorlight": "#000",
    "--sensorofflight": "#ccc",
    "--bordercolorlight": "#bbb",
    "--hovercolorlight": "#c0c0c0",
    "--activecolorlight": "#d0d0d0",
    "--focuscolorlight": "#c5c5c5"
    }

    };
  }


static styles = [css`
ha-card[color-scheme="light"] {
--maingrpbgcolor: var(--maingrpbgcolorlight,var(--card-background-color));
--containerbordercolor: var(--containerborderlight,var(--card-background-color));
--btngrpbgcolor:  var(--btngrpbgcolorlight,whitesmoke);
--buttonbgcolor: var(--buttonbgcolorlight,#d9dcdf);
--buttontextcolor: var(--buttontextcolorlight, var(--primary-text-color));
--lcdbgcolor: var(--lcdbgcolorlight,#859c99);
--lcdtextcolor: var(--lcdtextcolorlight,#222); 
--sensoroff: var(--sensorofflight,#ccc);
--bordercolor: var(--bordercolorlight,#bbb);
--hovercolor: var(--hovercolorlight,#c0c0c0);
--activecolor: var(--activecolorlight,#d0d0d0);
--focuscolor: var(--focuscolorlight,#c5c5c5);

}
/*
           --lcdbg: var(--lcdbgcolordark,var(--input-fill-color));
           --lcdtext: var(--lcdtextcolordark,var(--primary-text-color));
           --buttonbg: var(--buttonbgcolordark,var(--input-fill-color));
           --buttontext: var(--buttontextcolordark,var(--primary-color));
           --sensorlabel: var(--sensorlabelcolordark,var(--accent-color));
           --sensoroff: var(--sensoroffcolordark,var(--disabled-text-color));
           --buttonhover: var(--buttonhovercolordark,var(--outline-hover-color));
           --buttonactive: var(--buttonactivecolordark,var(--secondary-text-color));
           --buttonfocus: var(--buttonfocuscolordark,var(--outline-color));
           --bordercolor: var(--bordercolordark,var(--disabled-color));
*/
ha-card[color-scheme="dark"] {
--maingrpbgcolor: var(--maingrpbgcolordark,var(--card-background-color));
--btngrpbgcolor: var(--btngrpbgcolordark,var(--card-background-color));
--containerbordercolor: var(--containerborderdark,var(--card-background-color));
--buttonbgcolor: var(--buttonbgcolordark,#252525);
--buttontextcolor: var(--buttontextcolordark,#03a9f4);
--lcdbgcolor: var(--lcdbgcolordark,#859c99);
--lcdtextcolor: var(--lcdtextcolordark,#222);
--sensoroff: var(--sensoroffdark,#303030);
--bordercolor: var(--bordercolordark,#333);
--hovercolor: var(--hovercolordark,#444);
--activecolor: var(--activecolordark,#555);
--focuscolor: var(--focuscolordark,#454545);


}

        :host {
           width: 100%;
           --mdc-icon-size: 16px;
       }

    ha-card {
          width: 100%;
          position: relative;
          font-size: var(--base-unit);
          padding-bottom: 16px;

}
      h2 {
        padding-left: 16px;
        text-align: left;
        font-family: "Arial";
    }

   


i.keypad-icon {
  /* use !important to prevent issues with browser extensions that change fonts */
  font-family: 'keypad_icons' !important;
  speak: none;
  font-style: normal;
  font-weight: normal;
  font-variant: normal;
  text-transform: none;
  line-height: 1;

  /* Better Font Rendering =========== */
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
}

.icon-star:before {
  content: "\\e905";
 }
.icon-ac:before {
  content: "\\e90d";
}
.icon-trouble:before {
  content: "\\e90e";
}
.icon-armed:before {
  content: "\\e90f";
}
.icon-check:before {
  content: "\\e910";
}
.icon-thief:before {
  content: "\\e90b";
}
.icon-alert:before {
  content: "\\e90c";
}
.icon-sleep:before {
  content: "\\e90a";
}
.icon-bypass:before {
  content: "\\e909";
}
.icon-programming:before {
  content: "\\e908";
}
.icon-flame:before {
  content: "\\e907";
}
.icon-info:before {
  content: "\\e906";
}
.icon-stay_away:before {
  content: "\\e904";
}
.icon-stay_empty:before {
  content: "\\e903";
}
.icon-bell:before {
  content: "\\e900";
}
.icon-refresh:before {
  content: "\\e902";
}
.icon-exit:before {
  content: "\\e901";
}
    .flex-container {
            display: flex;
            justify-content: center;
      }

 
      .container {
        border: 1px solid var(--containerbordercolor);
        border-radius: 1em;
        padding: 4px;
        background-color: var(--maingrpbgcolor);
        margin: 5px;
        width: 100%;
        max-width: 350px;
  
      }
        .keypad_title {
          margin: auto;
          padding-bottom: 5px;
          display: flex;
          justify-content: center          
         }    


            div.virtual_lcd {
                flex: 1;
                /*font-family: "Dot Matrix", sans-serif;*/
                font-family: "Arial";
                background-color: var(--lcdbgcolor);
                font-size: 1.1rem;
                border-radius: 8px;
                text-align: center; 
                color: var(--lcdtextcolor);
                overflow: auto;
                margin: 5px 0px 5px 5px;
                padding: 5px;
                padding-top: 10px;
                justify-content: center; 
                border: 1px solid var(--bordercolor);
            
             }
    



       div#first_line {
        height: 1.5em;
      }
       div#second_line {
        height: 1.5em;
      }

       div#beep_line {
        height: 1.0em;
      }

      
      div#lcd_container {
        margin: 2px 0px 2px 0px;
        border: 1px solid var(--bordercolor);
        padding: 4px;
        background-color: var(--btngrpbgcolor);
        border-radius: 10px;
        white-space: nowrap;
        display: flex;
        justify-content: center    
  
      }
      button.keypad_button {
        border-radius: 1.5rem;
        background-color: var(--buttonbgcolor);
        border: 2px solid var(--bordercolor);
        color: var(--buttontextcolor);
        font-weight: bold;
      }

      button.keypad_button_pad {
        width: 32%;
        font-weight: bold;
        font-size: 0.8rem;
        background-color: var(--buttonbgcolor);
        border: 2px solid var(--bordercolor);
        min-height: 2.3rem;
        color: var(--buttontextcolor);
        line-height: 0.8;

      }

      button.keypad_button_small {
        font-weight: bold;
        font-size: 0.8rem;
        line-height: 0.8;
        background-color: var(--buttonbgcolor);
        border: 2px solid var(--bordercolor); 
        margin-top: 0.1rem;
 
        padding: 6px;
        padding-right: 8px;
        padding-left: 8px;  
      }


      button.keypad_button_slim {
        width: 100%;
        line-height: 0.8;
        font-weight: bold;
        min-height: 1.5rem;
        background-color: var(--buttonbgcolor);
        border: 2px solid var(--bordercolor); 
        margin-top: .7rem; 
             
      }

       .keypad_cmd_text {
        font-size: 6.5px;
        font-size: calc(0.4em + 0.2vw);
        font-style: italic; 
        margin-top: 1px;
        font-weight: bold;
       }

      button.keypad_button_control {
        background-color: var(--buttonbgcolor);
        line-height: 0.8;
        margin: auto;
      }

 

      div.keypad_button_row {
        margin: 2px;
        text-align: center;
        white-space: nowrap;
        margin-top: 4px;
        margin-bottom: 4px;

      }


      div.status_icons {
        text-align: center;
        margin: 0px 0px 0px 4px;
        flex: 0;
        color: grey;
      }

      div.status_icons i {
        display: block;
        margin: 4px;
      }


      div#right_buttons,
      div#keypad_container {
        margin: 2px 0px 2px 2px;
        border: 1px solid var(--bordercolor);
        border-radius: 8px;
        background-color: var(--btngrpbgcolor);
        padding: 2px;
      }

      div#left_buttons {
        border: 1px solid var(--bordercolor);
        border-radius: 8px;
        background-color: var(--btngrpbgcolor);
        padding: 2px;
        margin: 2px 0px 2px 0px;
      }

      .greenbullet {
        color: #28a745;
      }

      .redbullet {
        color: #dc3545;
      }
        i.keypad-icon.icon-star {
         font-size: calc(0.5rem * var(--scale));;
        }
  

      div#keypad_container {
        display: flex;
        flex-direction: column;
        width: 100%;
      }

      div#right_buttons {

      }

      div#buttons_area {
        display: flex;
        justify-content: center;
      }

      .btn{
        &:hover {
            background-color: var(--hovercolor);
            color: var(--buttontextcolor);
          }
        &:focus {
            background-color: var(--focuscolor);
            color: var(--buttontextcolor);
          }
        &:active {
            background-color: var(--activecolor);
            color: var(--buttontextcolor);
          }
        }

 
/*boot strap css */
.btn {
  display: inline-block;
  font-weight: 400;
  text-align: center;
  white-space: nowrap;
  vertical-align: middle;
  -webkit-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  user-select: none;
  padding: 0.375rem 0.75rem;
  font-size: 1rem;
  line-height: 1.5;
  border-radius: 0.25rem;
  transition: lscolor 0.15s ease-in-out, background-color 0.15s ease-in-out, border-color 0.15s ease-in-out, box-shadow 0.15s ease-in-out;
}

.btn:hover, .btn:focus {
  text-decoration: none;
}

.btn:focus, .btn.focus {
/*  outline: 0;
  box-shadow: 0 0 0 0.2rem rgba(0, 123, 255, 0.25);
*/
}

.btn.disabled, .btn:disabled {
  opacity: 0.65;
}

.btn:not(:disabled):not(.disabled) {
  cursor: pointer;
}

a.btn.disabled,
fieldset:disabled a.btn {
  pointer-events: none;
}


.btn-outline-dark:focus, .btn-outline-dark.focus {
 /* box-shadow: 0 0 0 0.2rem rgba(52, 58, 64, 0.5);*/
}


.btn-outline-dark.disabled, .btn-outline-dark:disabled {

  background-color: transparent;
}



.btn-outline-dark:not(:disabled):not(.disabled):active:focus, .btn-outline-dark:not(:disabled):not(.disabled).active:focus,
.show > .btn-outline-dark.dropdown-toggle:focus {
  box-shadow: 0 0 0 0.2rem rgba(52, 58, 64, 0.5);
}

`];
}

customElements.define('alarm-keypad-card-dsc', AlarmKeypadCard);
window.customCards = window.customCards || [];
window.customCards.push({
    type: "alarm-keypad-card-dsc",
    name: "DSC Alarm Keypad Card",
    description: "Emulate a dsc alarm keypad",
    preview: true,
});
