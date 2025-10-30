console.info("%c  ALARM-KEYPAD-CARD %c v0.4.2 ", "color: orange; font-weight: bold; background: black", "color: white; font-weight: bold; background: dimgray");

import {
    LitElement,
    html,
    css,
} from "https://unpkg.com/lit-element@4.0.0/lit-element.js?module";



class AlarmKeypadCard extends LitElement {

    render() {
        return html`
   <ha-card header="${this._title}" color-scheme="${this._scheme}">
        <div id="zoom" class='flex-container'  style="${this._scale}" @click="${this.stopPropagation}">
                 ${this._title ? "" : html`<div class=separarator></div>`}
              <div class='keypad'>
                ${this._view_display ? html`
                  <div class="keypad_display">
                    <div class="display_line" id="display_line1">${this._line1}</div>
                    <div class="display_line" id="display_line2">${this._line2}</div>
                  </div>`: ''}


                ${this._view_status ? html`
                <div class='pad icon_row'>
                    <div class='mdc-button mdc-icon' style='color:${this._status_A_state};'><div class="icon-label">${this._status_A}</div>
                        <ha-icon  id="icon-a" icon="${this._iconA}"  />
                    </div>
                    <div class='mdc-button  mdc-icon' style='color:${this._status_B_state};'><div class="icon-label">${this._status_B}</div>
                        <ha-icon id="icon-b" icon="${this._iconB}"/>
                    </div>
                    <div class='mdc-button  mdc-icon' style='color:${this._status_C_state};'><div class="icon-label">${this._status_C}</div>
                        <ha-icon id="icon-c" icon="${this._iconC}"/>
                    </div>
                    <div class='mdc-button mdc-icon' style='color:${this._status_D_state};'><div class="icon-label">${this._status_D}</div>
                        <ha-icon id="icon-d" icon="${this._iconD}"/>
                    </div>                    
                </div>`: ''}
                

                ${this._view_status2 ? html`
                <div class='pad icon-row'>
                    <div class='mdc-button  mdc-icon' style='color:${this._status_E_state};'><div class="icon-label">${this._status_E}</div>
                        <ha-icon id="icon-e" icon="${this._iconE}"/>
                    </div>
  
                    <div class='mdc-button  mdc-icon' style='color:${this._status_F_state};'><div class="icon-label">${this._status_F}</div>
                        <ha-icon id="icon-f" icon="${this._iconF}"/>
                    </div>

                    <div class='mdc-button  mdc-icon' style='color:${this._status_G_state};'><div class="icon-label">${this._status_G}</div>
                        <ha-icon id="icon-g" icon="${this._iconG}"/>
                    </div>
                    <div class='mdc-button mdc-icon' style='color:${this._status_H_state};'><div class="icon-label">${this._status_H}</div>
                        <ha-icon id="icon-h" icon="${this._iconH}"/>
                    </div>                    
                </div>`: ''}


                ${this._view_pad ? html`                
                  <div class="pad">
                  
                ${this._button_left ? html`  
                    <div>
                      <button
                        class='mdc-button mdc-button--outlined${this._button_disabled_A}'
                         state="A"
                       @click="${this.setState}"
                        title='${this._button_A}'>${this._button_A}
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined${this._button_disabled_B}'
                         state="B"
                       @click="${this.setState}"
                        title='${this._button_B}'>${this._button_B}
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined${this._button_disabled_C}'
                         state="C"
                       @click="${this.setState}"
                        title='${this._button_C}'>${this._button_C}
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined${this._button_disabled_D}'
                         state="D"
                       @click="${this.setState}"
                        title='${this._button_D}'>${this._button_D}
                      </button>
                     ${this._view_bottom ? html`                       
                     <button
                        class='mdc-button mdc-button--outlined${this._button_disabled_H}'
                         state="H"
                       @click="${this.setState}" @disabled="disabled"
                        title='${this._button_H}'>${this._button_H}
                     </button>`: ''}
                    </div>`: ''}    
                    
                    <div>
                      <button
                        class='mdc-button mdc-button--outlined'
                         state="1"
                       @click="${this.setState}"
                        title='1'>1<div class="keypad_cmd_text">${this._text_1}</div>
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                         state="4"
                       @click="${this.setState}"
                        title='4'>4<div class="keypad_cmd_text">${this._text_4}</div>
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                         state="7"
                       @click="${this.setState}"
                        title='7'>7<div class="keypad_cmd_text">${this._text_7}</div>
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                         state="*"
                       @click="${this.setState}"
                        title='*'>*<div class="keypad_cmd_text">${this._text_star}</div>
                      </button>

                     ${this._view_bottom ? html`   
                     <button
                        class='mdc-button mdc-button--outlined${this._button_disabled_E}'
                         state="E"
                       @click="${this.setState}"
                        title='${this._button_E}'>${this._button_E}
                     </button>`: ''}
                      
                    </div>

                    <div>
                      <button
                        class='mdc-button mdc-button--outlined'
                         state="2"
                       @click="${this.setState}"
                        title='2'>2<div class="keypad_cmd_text">${this._text_2}</div>
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                         state="5"
                       @click="${this.setState}"
                        title='5'>5<div class="keypad_cmd_text">${this._text_5}</div>
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                         state="8"
                       @click="${this.setState}"
                        title='8'>8<div class="keypad_cmd_text">${this._text_8}</div>
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                         state="0"
                       @click="${this.setState}"
                        title='0'>0<div class="keypad_cmd_text">${this._text_0}</div>
                      </button>

                     ${this._view_bottom ? html`                       
                     <button
                        class='mdc-button mdc-button--outlined${this._button_disabled_F}'
                         state="F"
                       @click="${this.setState}"
                        title='${this._button_F}'>${this._button_F}
                     </button>`: ''}
                    </div>

                    <div>
                      <button
                        class='mdc-button mdc-button--outlined'
                         state="3"
                       @click="${this.setState}"
                        title='3'>3<div class="keypad_cmd_text">${this._text_3}</div>
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                         state="6"
                       @click="${this.setState}"
                        title='6'>6<div class="keypad_cmd_text">${this._text_6}</div>
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                         state="9"
                       @click="${this.setState}"
                        title='9'>9<div class="keypad_cmd_text">${this._text_9}</div>
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                         state="#"
                       @click="${this.setState}"
                        title='#'>#<div class="keypad_cmd_text">${this._text_pound}</div>
                      </button>
                      ${this._view_bottom ? html`                      
                     <button
                        class='mdc-button mdc-button--outlined${this._button_disabled_G}'
                         state="G"
                       @click="${this.setState}"
                        title='${this._button_G}'>${this._button_G}
                      </button>`: ''}
                   
                    </div>
                   ${this._button_left ? '' : html`  
                    <div>
                      <button
                        class='mdc-button mdc-button--outlined${this._button_disabled_A}'
                         state="A"
                       @click="${this.setState}"
                        title='${this._button_A}'>${this._button_A}
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined${this._button_disabled_B}'
                         state="B"
                       @click="${this.setState}"
                        title='${this._button_B}'>${this._button_B}
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined${this._button_disabled_C}'
                         state="C"
                       @click="${this.setState}"
                        title='${this._button_C}'>${this._button_C}
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined${this._button_disabled_D}'
                         state="D"
                       @click="${this.setState}"
                        title='${this._button_D}'>${this._button_D}
                      </button>
                     ${this._view_bottom ? html`                       
                     <button
                        class='mdc-button mdc-button--outlined${this._button_disabled_H}'
                         state="H"
                       @click="${this.setState}"
                        title='${this._button_H}'>${this._button_H}
                     </button>`: ''}
                     
                    </div>`}
                    
                </div>`: ''}
              </div> <!--keypad -->
      </div>   <!-- zoom -->
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

    updated(changedProperties) {

        if (changedProperties.has("_kpdline1") || changedProperties.has("_kpdline2")) {
            this.displayChanged();
        }
        if (changedProperties.has("_kpdbeep")) {
            this.beepChanged();
        }

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
            _kpda: {
                type: Object,
            },
            _kpdb: {
                type: Object,
            },
            _kpdc: {
                type: Object,
            },
            _kpdd: {
                type: Object,
            },
            _kpde: {
                type: Object,
            },
            _kpdf: {
                type: Object,
            },
            _kpdg: {
                type: Object,
            },
            _kpdh: {
                type: Object,
            },
            _line1: String,
            _line2: String,


        };
    }

    setConfig(config) {
        if (!config.disp_line1) throw new Error('You need to define a disp_line1');
        if (!config.disp_line2) throw new Error('You need to define a disp_line2');
        if (!config.service_type) throw new Error('You need to define a service type');
        if (!config.service) throw new Error('You need to define a keypad service');
        this._config = config;
        this._title = config.title;
        this._view_display = (config.view_display != null) ? config.view_display : true;
        this._kpdservice = config.service;
        this._kpdservicetype = config.service_type;
        this._view_pad = (config.view_pad != null) ? config.view_pad : true;
        this._button_left = (config.button_left != null) ? config.button_left : false;
        this._view_bottom = (config.view_bottom != null) ? config.view_bottom : false;
        this._view_status = (config.view_status != null) ? config.view_status : true;
        this._view_status2 = (config.view_status_2 != null) ? config.view_status_2 : false;
        this._scale = (config.scale != null) ? "transform-origin: 0 0; zoom: " + config.scale + "; -moz-transform: scale(" + config.scale + ");" : "1";

        this._button_A = (config.button_A != null) ? config.button_A : "A";
        this._button_B = (config.button_B != null) ? config.button_B : "B";
        this._button_C = (config.button_C != null) ? config.button_C : "C";
        this._button_D = (config.button_D != null) ? config.button_D : "D";
        this._button_E = (config.button_E != null) ? config.button_E : "E";
        this._button_F = (config.button_F != null) ? config.button_F : "F";
        this._button_G = (config.button_G != null) ? config.button_G : "G";
        this._button_H = (config.button_H != null) ? config.button_H : "H";

        this._cmd_A = (config.cmd_A != null) ? config.cmd_A : "";
        this._cmd_B = (config.cmd_B != null) ? config.cmd_B : "";
        this._cmd_C = (config.cmd_C != null) ? config.cmd_C : "";
        this._cmd_D = (config.cmd_D != null) ? config.cmd_D : "";
        this._cmd_E = (config.cmd_E != null) ? config.cmd_E : "";
        this._cmd_F = (config.cmd_F != null) ? config.cmd_F : "";
        this._cmd_G = (config.cmd_G != null) ? config.cmd_G : "";
        this._cmd_H = (config.cmd_H != null) ? config.cmd_H : "";

        this._button_disabled_A = (config.button_disabled_A) || this._cmd_A == "disabled" ? " disabled" : "";
        this._button_disabled_B = (config.button_disabled_B) || this._cmd_B == "disabled" ? " disabled" : "";
        this._button_disabled_C = (config.button_disabled_C) || this._cmd_C == "disabled" ? " disabled" : "";
        this._button_disabled_D = (config.button_disabled_D) || this._cmd_D == "disabled" ? " disabled" : "";
        this._button_disabled_E = (config.button_disabled_E) || this._cmd_E == "disabled" ? " disabled" : "";
        this._button_disabled_F = (config.button_disabled_F) || this._cmd_F == "disabled" ? " disabled" : "";
        this._button_disabled_G = (config.button_disabled_G) || this._cmd_G == "disabled" ? " disabled" : "";
        this._button_disabled_H = (config.button_disabled_H) || this._cmd_H == "disabled" ? " disabled" : "";

        this._status_A = (config.status_A != null) ? config.status_A : "";
        this._status_B = (config.status_B != null) ? config.status_B : "";
        this._status_C = (config.status_C != null) ? config.status_C : "";
        this._status_D = (config.status_D != null) ? config.status_D : "";
        this._status_E = (config.status_E != null) ? config.status_E : "";
        this._status_F = (config.status_F != null) ? config.status_F : "";
        this._status_G = (config.status_G != null) ? config.status_G : "";
        this._status_H = (config.status_H != null) ? config.status_H : "";

        this._status_A_on_icon = (config.status_A_on_icon != null) ? config.status_A_on_icon : "mdi:check-circle-outline";
        this._status_A_off_icon = (config.status_A_off_icon != null) ? config.status_A_off_icon : "mdi:circle-outline";
        this._status_B_on_icon = (config.status_B_on_icon != null) ? config.status_B_on_icon : "mdi:check-circle-outline";
        this._status_B_off_icon = (config.status_B_off_icon != null) ? config.status_B_off_icon : "mdi:circle-outline";
        this._status_C_on_icon = (config.status_C_on_icon != null) ? config.status_C_on_icon : "mdi:check-circle-outline";
        this._status_C_off_icon = (config.status_C_off_icon != null) ? config.status_C_off_icon : "mdi:circle-outline";
        this._status_D_on_icon = (config.status_D_on_icon != null) ? config.status_D_on_icon : "mdi:check-circle-outline";
        this._status_D_off_icon = (config.status_D_off_icon != null) ? config.status_D_off_icon : "mdi:circle-outline";
        this._status_E_on_icon = (config.status_E_on_icon != null) ? config.status_E_on_icon : "mdi:check-circle-outline";
        this._status_E_off_icon = (config.status_E_off_icon != null) ? config.status_E_off_icon : "mdi:circle-outline";
        this._status_F_on_icon = (config.status_F_on_icon != null) ? config.status_F_on_icon : "mdi:check-circle-outline";
        this._status_F_off_icon = (config.status_F_off_icon != null) ? config.status_F_off_icon : "mdi:circle-outline";
        this._status_G_on_icon = (config.status_G_on_icon != null) ? config.status_G_on_icon : "mdi:check-circle-outline";
        this._status_G_off_icon = (config.status_G_off_icon != null) ? config.status_G_off_icon : "mdi:circle-outline";
        this._status_H_on_icon = (config.status_H_on_icon != null) ? config.status_H_on_icon : "mdi:check-circle-outline";
        this._status_H_off_icon = (config.status_H_off_icon != null) ? config.status_H_off_icon : "mdi:circle-outline";

        this._status_A_color = (config.status_A_color != null) ? config.status_A_color : "green";
        this._status_B_color = (config.status_B_color != null) ? config.status_B_color : "green";
        this._status_C_color = (config.status_C_color != null) ? config.status_C_color : "green";
        this._status_D_color = (config.status_D_color != null) ? config.status_D_color : "green";
        this._status_E_color = (config.status_E_color != null) ? config.status_E_color : "green";
        this._status_F_color = (config.status_F_color != null) ? config.status_F_color : "green";
        this._status_G_color = (config.status_G_color != null) ? config.status_G_color : "green";
        this._status_H_color = (config.status_H_color != null) ? config.status_H_color : "green";

        this._key_0 = (config.key_0 != null) ? config.key_0 : "";
        this._key_1 = (config.key_1 != null) ? config.key_1 : "";
        this._key_2 = (config.key_2 != null) ? config.key_2 : "";
        this._key_3 = (config.key_3 != null) ? config.key_3 : "";
        this._key_4 = (config.key_4 != null) ? config.key_4 : "";
        this._key_5 = (config.key_5 != null) ? config.key_5 : "";
        this._key_6 = (config.key_6 != null) ? config.key_6 : "";
        this._key_7 = (config.key_7 != null) ? config.key_7 : "";
        this._key_8 = (config.key_8 != null) ? config.key_8 : "";
        this._key_9 = (config.key_9 != null) ? config.key_9 : "";

        this._key_star = (config.key_star != null) ? config.key_star : "";
        this._key_pound = (config.key_pound != null) ? config.key_pound : "";

        this._text_0 = (config.text_0 != null) ? config.text_0 : "";
        this._text_1 = (config.text_1 != null) ? config.text_1 : "";
        this._text_2 = (config.text_2 != null) ? config.text_2 : "";
        this._text_3 = (config.text_3 != null) ? config.text_3 : "";
        this._text_4 = (config.text_4 != null) ? config.text_4 : "";
        this._text_5 = (config.text_5 != null) ? config.text_5 : "";
        this._text_6 = (config.text_6 != null) ? config.text_6 : "";
        this._text_7 = (config.text_7 != null) ? config.text_7 : "";
        this._text_8 = (config.text_8 != null) ? config.text_8 : "";
        this._text_9 = (config.text_9 != null) ? config.text_9 : "";
        this._text_star = (config.text_star != null) ? config.text_star : "";
        this._text_pound = (config.text_pound != null) ? config.text_pound : "";

        this._vibrate = (config.vibration_duration != null) ? config.vibration_duration : 5;

        this._style = config.style != null ? config.style : "";
        for (let i in this._style) {
            if (this._style[i] == null) continue;
            var v = this._style[i].replace(/;/gi, '');
            //console.log(i,v);
            this.style.setProperty(i, v);
        }

    }

    set hass(hass) {
        this._hass = hass;
        this._scheme = hass.themes.darkMode ? "dark" : "light";
        this._kpdline1 = this._hass.states[this._config.disp_line1];
        this._kpdline2 = this._hass.states[this._config.disp_line2];
        this._kpdbeep = this._hass.states[this._config.beep];
        this._kpda = this._hass.states[this._config.sensor_A];
        this._kpdb = this._hass.states[this._config.sensor_B];
        this._kpdc = this._hass.states[this._config.sensor_C];
        this._kpdd = this._hass.states[this._config.sensor_D];
        this._kpde = this._hass.states[this._config.sensor_E];
        this._kpdf = this._hass.states[this._config.sensor_F];
        this._kpdg = this._hass.states[this._config.sensor_G];
        this._kpdh = this._hass.states[this._config.sensor_H];

        this._iconA = this._kpda ? (this._kpda.state.toLowerCase() == "on" || this._kpda.state == "1") ? this._status_A_on_icon : this._status_A_off_icon : "";
        this._iconB = this._kpdb ? (this._kpdb.state.toLowerCase() == "on" || this._kpdb.state == "1") ? this._status_B_on_icon : this._status_B_off_icon : "";
        this._iconC = this._kpdc ? (this._kpdc.state.toLowerCase() == "on" || this._kpdc.state == "1") ? this._status_C_on_icon : this._status_C_off_icon : "";
        this._iconD = this._kpdd ? (this._kpdd.state.toLowerCase() == "on" || this._kpdd.state == "1") ? this._status_D_on_icon : this._status_D_off_icon : "";
        this._iconE = this._kpde ? (this._kpde.state.toLowerCase() == "on" || this._kpde.state == "1") ? this._status_E_on_icon : this._status_E_off_icon : "";
        this._iconF = this._kpdf ? (this._kpdf.state.toLowerCase() == "on" || this._kpdf.state == "1") ? this._status_F_on_icon : this._status_F_off_icon : "";
        this._iconG = this._kpdg ? (this._kpdg.state.toLowerCase() == "on" || this._kpdg.state == "1") ? this._status_G_on_icon : this._status_G_off_icon : "";
        this._iconH = this._kpdh ? (this._kpdh.state.toLowerCase() == "on" || this._kpdh.state == "1") ? this._status_H_on_icon : this._status_H_off_icon : "";

        this._status_A_state = this._kpda ? (this._kpda.state.toLowerCase() == "on" || this._kpda.state == "1") ? this._status_A_color : '' : "";
        this._status_B_state = this._kpdb ? (this._kpdb.state.toLowerCase() == "on" || this._kpdb.state == "1") ? this._status_B_color : '' : "";
        this._status_C_state = this._kpdc ? (this._kpdc.state.toLowerCase() == "on" || this._kpdc.state == "1") ? this._status_C_color : '' : "";
        this._status_D_state = this._kpdd ? (this._kpdd.state.toLowerCase() == "on" || this._kpdd.state == "1") ? this._status_D_color : '' : "";

        this._status_E_state = this._kpde ? (this._kpde.state.toLowerCase() == "on" || this._kpde.state == "1") ? this._status_E_color : '' : "";
        this._status_F_state = this._kpdf ? (this._kpdf.state.toLowerCase() == "on" || this._kpdf.state == "1") ? this._status_F_color : '' : "";
        this._status_G_state = this._kpdg ? (this._kpdg.state.toLowerCase() == "on" || this._kpdg.state == "1") ? this._status_G_color : '' : "";
        this._status_H_state = this._kpdh ? (this._kpdh.state.toLowerCase() == "on" || this._kpdh.state == "1") ? this._status_H_color : '' : "";


    }

    displayChanged() {
        let state1 = "";
        let state2 = "";
        if (this._kpdline1  != undefined)
            for (let i = 0; i < this._kpdline1.state.length; i++) state1 += this._translateChar(this._kpdline1.state[i]);
        if (this._kpdline2 != undefined)
            for (let i = 0; i < this._kpdline2.state.length; i++) state2 += this._translateChar(this._kpdline2.state[i]);
        this._line1 = state1;
        this._line2 = state2;
    }

    beepChanged() {
        if (this._kpdbeep.state == "0" || this._kpdbeep.state == null) {
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


    _translateChar(c) {
        // if (c.match('à') !== null ) return '<span class="blink">' + c + '</span>';
        // if (c.match('á') !== null ) return '<span class="under">' + c + '</span>';
        if (c.match('è') !== null) return 'e';
        if (c.match('é') !== null) return 'e';

        return c;
    }

    stopPropagation(e) {
        e.stopPropagation();
    }

    setState(e) {
        var key = e.currentTarget.getAttribute('state');

        switch (key) {
            case 'A': key = this._cmd_A; break;
            case 'B': key = this._cmd_B; break;
            case 'C': key = this._cmd_C; break;
            case 'D': key = this._cmd_D; break;
            case 'E': key = this._cmd_E; break;
            case 'F': key = this._cmd_F; break;
            case 'G': key = this._cmd_G; break;
            case 'H': key = this._cmd_H; break;
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
            default: return;
        }
        if (key == "" || key == "disabled") return;
        if ('vibrate' in navigator) {
            navigator.vibrate(this._vibrate);
        }
        this._hass.callService(this._kpdservicetype, this._kpdservice, key);

    }

    getCardSize() {
        let size = 2;
        if (this._config.view_pad) size += 4;     // 550px - 190px / 50
        return size;
    }

    static getStubConfig() {
        return {
            title: "Alarm Keypad",
            disp_line1: "sensor.<devicename>_line1_partition_1",
            disp_line2: "sensor.<devicename>_line2_partition_1",
            beep: "binary_sensor.<devicename>_beeps_partition_1",
            service_type: "esphome",
            service: "<devicename>_alarm_keypress_partition",
            status_A: "READY",
            status_B: "ARMED",
            status_C: "TROUBLE",
            status_D: "AC",
            sensor_A: "binary_sensor.<devicename>_ready_partition_1",
            sensor_B: "binary_sensor.<devicename>_armed_partition_1",
            sensor_C: "binary_sensor.<devicename>_trouble_status",
            sensor_D: "binary_sensor.<devicename>_ac_status",
            status_A_off_icon: "mdi:circle-off-outline",
            status_A_on_icon: "mdi:check-circle",
            status_A_color: "green",
            status_B_on_icon: "mdi:shield-lock",
            status_B_off_icon: "mdi:shield-off-outline",
            status_B_color: "red",
            status_C_on_icon: "mdi:alert",
            status_C_off_icon: "mdi:triangle-outline",
            status_C_color: "orange",
            status_D_on_icon: "mdi:power-plug",
            status_D_off_icon: "mdi:power-plug-off-outline",
            status_D_color: "green",
            button_A: "STAY",
            button_B: "AWAY",
            button_C: "NIGHT",
            button_D: "CHIME",
            button_E: "",
            button_F: "<",
            button_G: ">",
            button_H: "",
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
            text_0: "",
            cmd_A: {
                keys: "S",
                partition: "1"
            },
            cmd_B: {
                keys: "W",
                partition: "1"
            },
            cmd_C: {
                keys: "N",
                partition: "1"
            },
            cmd_D: {
                keys: "C",
                partition: "1"
            },
            cmd_F: {
                keys: "<",
                partition: "1"
            },
            cmd_G: {
                keys: ">",
                partition: "1"
            },
            key_0: {
                keys: "0",
                partition: "1"
            },
            key_1: {
                keys: "1",
                partition: "1"
            },
            key_2: {
                keys: "2",
                partition: "1"
            },
            key_3: {
                keys: "3",
                partition: "1"
            },
            key_4: {
                keys: "4",
                partition: "1"
            },
            key_5: {
                keys: "5",
                partition: "1"
            },
            key_6: {
                keys: "6",
                partition: "1"
            },
            key_7: {
                keys: "7",
                partition: "1"
            },
            key_8: {
                keys: "8",
                partition: "1"
            },
            key_9: {
                keys: "9",
                partition: "1"
            },
            key_star: {
                keys: "*",
                partition: "1"
            },
            key_pound: {
                keys: "#",
                partition: "1"
            },
            button_disabled_A: false,
            button_disabled_B: false,
            button_disabled_C: false,
            button_disabled_D: false,
            button_disabled_E: true,
            button_disabled_F: false,
            button_disabled_G: false,
            button_disabled_H: true,
            scale: 1,
            button_left: false,
            view_bottom: true,
            vibration_duration: 5,
/*
            style:
            {
                "--lcdbgcolordark": "var(--input-fill-color)",
                "--lcdtextcolordark": "var(--primary-text-color)",
                "--buttonbgcolordark": "var(--input-fill-color)",
                "--buttontextcolordark": "var(--primary-color)",
                "--buttonhovercolordark": "var(--outline-hover-color)",
                "--buttonactivecolordark": "var(--secondary-text-color)",
                "--sensorlabelcolordark": "var(--accent-color)",
                "--buttonfocuscolordark": "var(--outline-color)",
                "--sensoroffcolordark": "var(--disabled-text-color)",
                "--bordercolordark": "var(--disabled-color)",

                "--sensoroffcolorlight": "var(--disabled-text-color)",
                "--lcdbgcolorlight": "var(--input-fill-color)",
                "--lcdtextcolorlight": "var(--primary-text-color)",
                "--sensorlabelcolorlight": "var(--accent-color)",
                "--buttonbgcolorlight": "var(--input-fill-color)",
                "--buttontextcolorlight": "var(--primary-color)",
                "--buttonhovercolorlight": "var(--outline-hover-color)",
                "--buttonactivecolorlight": "var(--secondary-text-color)",
                "--buttonfocuscolorlight": "var(--outline-color)",
                "--bordercolorlight": "var(--disabled-color)"
            },
*/
        }
    }

    static get styles() {
        return css`

        @keyframes mdc-ripple-fg-radius-in{from{animation-timing-function:cubic-bezier(0.4, 0, 0.2, 1);transform:translate(var(--mdc-ripple-fg-translate-start, 0)) scale(1)}to{transform:translate(var(--mdc-ripple-fg-translate-end, 0)) scale(var(--mdc-ripple-fg-scale, 1))}}
        @keyframes mdc-ripple-fg-opacity-in{from{animation-timing-function:linear;opacity:0}to{opacity:var(--mdc-ripple-fg-opacity, 0)}}
        @keyframes mdc-ripple-fg-opacity-out{from{animation-timing-function:linear;opacity:var(--mdc-ripple-fg-opacity, 0)}to{opacity:0}}.mdc-ripple-surface--test-edge-var-bug{--mdc-ripple-surface-test-edge-var: 1px solid #000;visibility:hidden}.mdc-ripple-surface--test-edge-var-bug::before{border:var(--mdc-ripple-surface-test-edge-var)}.mdc-button{font-family:Roboto,sans-serif;-moz-osx-font-smoothing:grayscale;-webkit-font-smoothing:antialiased;font-size:.875rem;line-height:2.25rem;font-weight:500;letter-spacing:.0892857143em;text-decoration:none;text-transform:uppercase;--mdc-ripple-fg-size: 0;--mdc-ripple-left: 0;--mdc-ripple-top: 0;--mdc-ripple-fg-scale: 1;--mdc-ripple-fg-translate-end: 0;--mdc-ripple-fg-translate-start: 0;-webkit-tap-highlight-color:rgba(0,0,0,0);will-change:transform,opacity;padding:0 8px 0 8px;display:inline-flex;position:relative;align-items:center;justify-content:center;box-sizing:border-box;min-width1:64px;height:36px;border:none;outline:none;line-height:inherit;user-select:none;-webkit-appearance:none;overflow:hidden;vertical-align:middle;border-radius:4px}.mdc-button::before,.mdc-button::after{position:absolute;border-radius:50%;opacity:0;pointer-events:none;content:""}.mdc-button::before{transition:opacity 15ms linear,background-color 15ms linear;z-index:1}.mdc-button.mdc-ripple-upgraded::before{transform:scale(var(--mdc-ripple-fg-scale, 1))}.mdc-button.mdc-ripple-upgraded::after{top:0;left:0;transform:scale(0);transform-origin:center center}.mdc-button.mdc-ripple-upgraded--unbounded::after{top:var(--mdc-ripple-top, 0);left:var(--mdc-ripple-left, 0)}.mdc-button.mdc-ripple-upgraded--foreground-activation::after{animation:225ms mdc-ripple-fg-radius-in forwards,75ms mdc-ripple-fg-opacity-in forwards}.mdc-button.mdc-ripple-upgraded--foreground-deactivation::after{animation:150ms mdc-ripple-fg-opacity-out;transform:translate(var(--mdc-ripple-fg-translate-end, 0)) scale(var(--mdc-ripple-fg-scale, 1))}.mdc-button::before,.mdc-button::after{top:calc(50% - 100%);left:calc(50% - 100%);width1:200%;height:200%}.mdc-button.mdc-ripple-upgraded::after{width:var(--mdc-ripple-fg-size, 100%);height:var(--mdc-ripple-fg-size, 100%)}.mdc-button::-moz-focus-inner{padding:0;border:0}.mdc-button:active{outline:none}.mdc-button:hover{cursor:pointer}.mdc-button:disabled{background-color:transparent;color:rgba(0,0,0,.38);cursor:default;pointer-events:none}.mdc-button.mdc-button--dense{border-radius:4px}.mdc-button:not(:disabled){background-color:transparent}.mdc-button:not(:disabled){color:rgba(0,0,0,.38);color:var(--mdc-theme-primary, rgba(0,0,0,.38))}.mdc-button::before,.mdc-button::after{background-color:rgba(0,0,0,.38)}@supports not (-ms-ime-align: auto){.mdc-button::before,.mdc-button::after{background-color:var(--mdc-ripple-color, #03A9F4)}}.mdc-button:hover::before{opacity:.04}.mdc-button:not(.mdc-ripple-upgraded):focus(disabled)::before,.mdc-button.mdc-ripple-upgraded--background-focused::before{transition-duration:75ms;opacity:.12}.mdc-button:not(.mdc-ripple-upgraded)::after{transition:opacity 150ms linear}.mdc-button:not(.mdc-ripple-upgraded):active(disabled)::after{transition-duration:75ms;opacity:.64}.mdc-button.mdc-ripple-upgraded{--mdc-ripple-fg-opacity: 0.16}.mdc-button .mdc-button__icon{margin-left:0;margin-right:8px;display:inline-block;width:18px;height:18px;font-size:18px;vertical-align:top}[dir=rtl] .mdc-button .mdc-button__icon,.mdc-button .mdc-button__icon[dir=rtl]{margin-left:8px;margin-right:0}.mdc-button svg.mdc-button__icon{fill:rgba(0,0,0,.38)}.mdc-button--raised .mdc-button__icon,.mdc-button--unelevated .mdc-button__icon,.mdc-button--outlined .mdc-button__icon{margin-left:-4px;margin-right:8px}[dir=rtl] .mdc-button--raised .mdc-button__icon,.mdc-button--raised .mdc-button__icon[dir=rtl],[dir=rtl] .mdc-button--unelevated .mdc-button__icon,.mdc-button--unelevated .mdc-button__icon[dir=rtl],[dir=rtl] .mdc-button--outlined .mdc-button__icon,.mdc-button--outlined .mdc-button__icon[dir=rtl]{margin-left:8px;margin-right:-4px}.mdc-button--raised,.mdc-button--unelevated{padding:0 16px 0 16px}.mdc-button--raised:disabled,.mdc-button--unelevated:disabled{background-color:rgba(0,0,0,.12);color:rgba(0,0,0,.37)}.mdc-button--raised:not(:disabled),.mdc-button--unelevated:not(:disabled){background-color:rgba(0,0,0,.38)}@supports not (-ms-ime-align: auto){.mdc-button--raised:not(:disabled),.mdc-button--unelevated:not(:disabled){background-color:var(--mdc-theme-primary, rgba(0,0,0,.38))}}.mdc-button--raised:not(:disabled),.mdc-button--unelevated:not(:disabled){color:#fff;color:var(--mdc-theme-on-primary, #fff)}.mdc-button--raised::before,.mdc-button--raised::after,.mdc-button--unelevated::before,.mdc-button--unelevated::after{background-color:#fff}@supports not (-ms-ime-align: auto){.mdc-button--raised::before,.mdc-button--raised::after,.mdc-button--unelevated::before,.mdc-button--unelevated::after{background-color:var(--mdc-theme-on-primary, #fff)}}.mdc-button--raised:hover::before,.mdc-button--unelevated:hover::before{opacity:.08}.mdc-button--raised:not(.mdc-ripple-upgraded):focus::before,.mdc-button--raised.mdc-ripple-upgraded--background-focused::before,.mdc-button--unelevated:not(.mdc-ripple-upgraded):focus::before,.mdc-button--unelevated.mdc-ripple-upgraded--background-focused::before{transition-duration:75ms;opacity:.24}.mdc-button--raised:not(.mdc-ripple-upgraded)::after,.mdc-button--unelevated:not(.mdc-ripple-upgraded)::after{transition:opacity 150ms linear}.mdc-button--raised:not(.mdc-ripple-upgraded):active1::after,.mdc-button--unelevated:not(.mdc-ripple-upgraded):active::after{transition-duration:75ms;opacity:.32}.mdc-button--raised.mdc-ripple-upgraded,.mdc-button--unelevated.mdc-ripple-upgraded{--mdc-ripple-fg-opacity: 0.32}.mdc-button--raised{box-shadow:0px 3px 1px -2px rgba(0, 0, 0, 0.2),0px 2px 2px 0px rgba(0, 0, 0, 0.14),0px 1px 5px 0px rgba(0,0,0,.12);transition:box-shadow 280ms cubic-bezier(0.4, 0, 0.2, 1)}.mdc-button--raised:hover,.mdc-button--raised:focus{box-shadow:0px 2px 4px -1px rgba(0, 0, 0, 0.2),0px 4px 5px 0px rgba(0, 0, 0, 0.14),0px 1px 10px 0px rgba(0,0,0,.12)}.mdc-button--raised:activebox-shadow:0px 5px 5px -3px rgba(0, 0, 0, 0.2),0px 8px 10px 1px rgba(0, 0, 0, 0.14),0px 3px 14px 2px rgba(0,0,0,.12)}.mdc-button--raised:disabled{box-shadow:0px 0px 0px 0px rgba(0, 0, 0, 0.2),0px 0px 0px 0px rgba(0, 0, 0, 0.14),0px 0px 0px 0px rgba(0,0,0,.12)}.mdc-button--outlined{border-style:solid;padding:8px 8px 8px 8px;border-width:1px}.mdc-button--outlined:disabled{border-color:rgba(0,0,0,.37)}.mdc-button--outlined:not(:disabled){border-color:rgba(0,0,0,.38);border-color:var(--mdc-button-outline-color, rgba(0,0,0,.38))}.mdc-button--dense{height:32px;font-size:.8125rem}.material-icons{font-family:var(--mdc-icon-font, "Material Icons");font-weight:normal;font-style:normal;font-size:var(--mdc-icon-size, 24px);line-height:1;letter-spacing:normal;text-transform:none;display:inline-block;white-space:nowrap;word-wrap:normal;direction:ltr;-webkit-font-feature-settings:"liga";-webkit-font-smoothing:antialiased}:host{display:inline-flex;outline:none}.mdc-button{flex:1}


      :host {
           width: 100%;
           --mdc-icon-size: 18px;
       }

ha-card[color-scheme="dark"] {
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
}

ha-card[color-scheme="light"] {
           --lcdbg: var(--lcdbgcolorlight,var(--input-fill-color));
           --lcdtext: var(--lcdtextcolorlight,var(--primary-text-color));
           --buttonbg: var(--buttonbgcolorlight,var(--input-fill-color));
           --buttontext: var(--buttontextcolorlight,var(--primary-color));
           --sensorlabel: var(--sensorlabelcolorlight,var(--accent-color));
           --sensoroff: var(--sensoroffcolorlight,var(--disabled-text-color));
           --buttonhover: var(--buttonhovercolorlight,var(--outline-hover-color));
           --buttonactive: var(--buttonactivecolorlight,var(--secondary-text-color));
           --buttonfocus: var(--buttonfocuscolorlight,var(--outline-color));
           --bordercolor:  var(--bordercolorlight,var(--disabled-color));
}


       ha-card {
          width: 100%;
          position: relative;
          font-size: var(--base-unit);
          padding-bottom: 16px;
        }

        .flex-container {
            display: flex;
            justify-content: center;
            align-items: center;
            flex-direction: column;
            width: 100%;
        }
        
        .keypad {
           width: 100%;
           max-width: 300px;
        }
        
        .icon_row {
            width: 100%;
          }
        .keypad_display {
          background-color: var(--lcdbg);
          border-radius: 10px;
          min-height: 40px;
          margin: auto;
          margin-bottom: 3px;
          border: 1px solid var(--bordercolor);
          overflow: auto;
          padding: 8px;
          max-width: 300px;
          width: 90%;
         
        }
        .display_line {
          font-size: 1rem;
          line-height: 1.1;
          color: var(--lcdtext);
          font-family: monospace;
          display: flex;
          justify-content: center;
        }
        #display_line1 {
          padding-bottom: 10px;
          white-space: pre-wrap;
        }
        #display_line2 {
          white-space: pre-wrap;
        } 

        .pad {
          display: flex;
          justify-content: center;
          width: 100%;
        }
        .pad div {
          flex-direction: column;
        }

        .mdc-button {
    
          margin-top: 8px;
          margin-right: 8px;
          margin-bottom: 8px;
          margin-left: 8px;
           width: 75%;

        }
        
        .mdc-button--outlined:not(:disabled) {
           border: 1px solid var(--bordercolor);
           color: var(--buttontext);
           background-color: var(--buttonbg);
           font-size: 0.8rem;
           display: flex;


        }
        
        .mdc-button--outlined:hover {
          background-color: var(--buttonhover);  
        }
        
        .mdc-button--outlined:active {
          background-color: var(--buttonactive) !important;  
        }

        .mdc-button--outlined:focus {
         background-color: var(--buttonfocus) ;
        }

        .mdc-button--outlined.disabled {
              border: 0px;
        }
        
        .mdc-button.disabled {
            background-color: transparent;
            cursor: default;
            pointer-events: none;
        }
        
        .mdc-icon {
          height: 40px;
          margin-top: 4px;
          margin-right: 4px;
          margin-bottom: 4px;
          margin-left: 4px;
        }

        ha-icon {
            

        }
        
        .icon-label:not(:disabled) {
         color: var(--sensorlabel);
         font-size: 0.53rem;
        }
        
        .mdc-icon:not(:disabled) {
          color: var(--sensoroff);
        }
        
        .bottom {
          padding-left: 2px;
          text-align:center;
          justify-content: center;
          margin: auto;
        }

        .under {
          text-decoration: underline;
        }
  
        /* text blinking */
        .blink{
          animation:blinkingText 1.2s infinite;
        }
  
        @keyframes blinkingText{
            0%  { color: #000;        }
            49% { color: #000;        }
            60% { color: transparent; }
            99% { color:transparent;  }
            100%{ color: #000;        }
        }
        
       .keypad_cmd_text {
        font-size: 0.5rem;
        font-style: italic; 
        padding-left: .2rem;
       }        

    `;
    }


}


customElements.define('alarm-keypad-card', AlarmKeypadCard);

window.customCards = window.customCards || [];
window.customCards.push({
    type: "alarm-keypad-card",
    name: "Alarm Keypad Card",
    description: "Emulate a dsc or vista alarm keypad",
    preview: true,
});


