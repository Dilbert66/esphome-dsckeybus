console.info("%c  Alarm-keypad-card  \n%c Version 0.0.4a ", "color: orange; font-weight: bold; background: black", "color: white; font-weight: bold; background: dimgray");

class AlarmKeypadCard extends Polymer.Element {

  static get template() {
    
    return Polymer.html`
      <style>
        ha-card {  
          padding-bottom: 16px;
          position: relative;
          font-size: calc(var(--base-unit));
        }

        .flex-container {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        @keyframes mdc-ripple-fg-radius-in{from{animation-timing-function:cubic-bezier(0.4, 0, 0.2, 1);transform:translate(var(--mdc-ripple-fg-translate-start, 0)) scale(1)}to{transform:translate(var(--mdc-ripple-fg-translate-end, 0)) scale(var(--mdc-ripple-fg-scale, 1))}}
        @keyframes mdc-ripple-fg-opacity-in{from{animation-timing-function:linear;opacity:0}to{opacity:var(--mdc-ripple-fg-opacity, 0)}}
        @keyframes mdc-ripple-fg-opacity-out{from{animation-timing-function:linear;opacity:var(--mdc-ripple-fg-opacity, 0)}to{opacity:0}}.mdc-ripple-surface--test-edge-var-bug{--mdc-ripple-surface-test-edge-var: 1px solid #000;visibility:hidden}.mdc-ripple-surface--test-edge-var-bug::before{border:var(--mdc-ripple-surface-test-edge-var)}.mdc-button{font-family:Roboto,sans-serif;-moz-osx-font-smoothing:grayscale;-webkit-font-smoothing:antialiased;font-size:.875rem;line-height:2.25rem;font-weight:500;letter-spacing:.0892857143em;text-decoration:none;text-transform:uppercase;--mdc-ripple-fg-size: 0;--mdc-ripple-left: 0;--mdc-ripple-top: 0;--mdc-ripple-fg-scale: 1;--mdc-ripple-fg-translate-end: 0;--mdc-ripple-fg-translate-start: 0;-webkit-tap-highlight-color:rgba(0,0,0,0);will-change:transform,opacity;padding:0 8px 0 8px;display:inline-flex;position:relative;align-items:center;justify-content:center;box-sizing:border-box;min-width:64px;height:36px;border:none;outline:none;line-height:inherit;user-select:none;-webkit-appearance:none;overflow:hidden;vertical-align:middle;border-radius:4px}.mdc-button::before,.mdc-button::after{position:absolute;border-radius:50%;opacity:0;pointer-events:none;content:""}.mdc-button::before{transition:opacity 15ms linear,background-color 15ms linear;z-index:1}.mdc-button.mdc-ripple-upgraded::before{transform:scale(var(--mdc-ripple-fg-scale, 1))}.mdc-button.mdc-ripple-upgraded::after{top:0;left:0;transform:scale(0);transform-origin:center center}.mdc-button.mdc-ripple-upgraded--unbounded::after{top:var(--mdc-ripple-top, 0);left:var(--mdc-ripple-left, 0)}.mdc-button.mdc-ripple-upgraded--foreground-activation::after{animation:225ms mdc-ripple-fg-radius-in forwards,75ms mdc-ripple-fg-opacity-in forwards}.mdc-button.mdc-ripple-upgraded--foreground-deactivation::after{animation:150ms mdc-ripple-fg-opacity-out;transform:translate(var(--mdc-ripple-fg-translate-end, 0)) scale(var(--mdc-ripple-fg-scale, 1))}.mdc-button::before,.mdc-button::after{top:calc(50% - 100%);left:calc(50% - 100%);width:200%;height:200%}.mdc-button.mdc-ripple-upgraded::after{width:var(--mdc-ripple-fg-size, 100%);height:var(--mdc-ripple-fg-size, 100%)}.mdc-button::-moz-focus-inner{padding:0;border:0}.mdc-button:active{outline:none}.mdc-button:hover{cursor:pointer}.mdc-button:disabled{background-color:transparent;color:rgba(0,0,0,.37);cursor:default;pointer-events:none}.mdc-button.mdc-button--dense{border-radius:4px}.mdc-button:not(:disabled){background-color:transparent}.mdc-button:not(:disabled){color:#6200ee;color:var(--mdc-theme-primary, #6200ee)}.mdc-button::before,.mdc-button::after{background-color:#6200ee}@supports not (-ms-ime-align: auto){.mdc-button::before,.mdc-button::after{background-color:var(--mdc-ripple-color, #03A9F4)}}.mdc-button:hover::before{opacity:.04}.mdc-button:not(.mdc-ripple-upgraded):focus::before,.mdc-button.mdc-ripple-upgraded--background-focused::before{transition-duration:75ms;opacity:.12}.mdc-button:not(.mdc-ripple-upgraded)::after{transition:opacity 150ms linear}.mdc-button:not(.mdc-ripple-upgraded):active::after{transition-duration:75ms;opacity:.16}.mdc-button.mdc-ripple-upgraded{--mdc-ripple-fg-opacity: 0.16}.mdc-button .mdc-button__icon{margin-left:0;margin-right:8px;display:inline-block;width:18px;height:18px;font-size:18px;vertical-align:top}[dir=rtl] .mdc-button .mdc-button__icon,.mdc-button .mdc-button__icon[dir=rtl]{margin-left:8px;margin-right:0}.mdc-button svg.mdc-button__icon{fill:currentColor}.mdc-button--raised .mdc-button__icon,.mdc-button--unelevated .mdc-button__icon,.mdc-button--outlined .mdc-button__icon{margin-left:-4px;margin-right:8px}[dir=rtl] .mdc-button--raised .mdc-button__icon,.mdc-button--raised .mdc-button__icon[dir=rtl],[dir=rtl] .mdc-button--unelevated .mdc-button__icon,.mdc-button--unelevated .mdc-button__icon[dir=rtl],[dir=rtl] .mdc-button--outlined .mdc-button__icon,.mdc-button--outlined .mdc-button__icon[dir=rtl]{margin-left:8px;margin-right:-4px}.mdc-button--raised,.mdc-button--unelevated{padding:0 16px 0 16px}.mdc-button--raised:disabled,.mdc-button--unelevated:disabled{background-color:rgba(0,0,0,.12);color:rgba(0,0,0,.37)}.mdc-button--raised:not(:disabled),.mdc-button--unelevated:not(:disabled){background-color:#6200ee}@supports not (-ms-ime-align: auto){.mdc-button--raised:not(:disabled),.mdc-button--unelevated:not(:disabled){background-color:var(--mdc-theme-primary, #6200ee)}}.mdc-button--raised:not(:disabled),.mdc-button--unelevated:not(:disabled){color:#fff;color:var(--mdc-theme-on-primary, #fff)}.mdc-button--raised::before,.mdc-button--raised::after,.mdc-button--unelevated::before,.mdc-button--unelevated::after{background-color:#fff}@supports not (-ms-ime-align: auto){.mdc-button--raised::before,.mdc-button--raised::after,.mdc-button--unelevated::before,.mdc-button--unelevated::after{background-color:var(--mdc-theme-on-primary, #fff)}}.mdc-button--raised:hover::before,.mdc-button--unelevated:hover::before{opacity:.08}.mdc-button--raised:not(.mdc-ripple-upgraded):focus::before,.mdc-button--raised.mdc-ripple-upgraded--background-focused::before,.mdc-button--unelevated:not(.mdc-ripple-upgraded):focus::before,.mdc-button--unelevated.mdc-ripple-upgraded--background-focused::before{transition-duration:75ms;opacity:.24}.mdc-button--raised:not(.mdc-ripple-upgraded)::after,.mdc-button--unelevated:not(.mdc-ripple-upgraded)::after{transition:opacity 150ms linear}.mdc-button--raised:not(.mdc-ripple-upgraded):active::after,.mdc-button--unelevated:not(.mdc-ripple-upgraded):active::after{transition-duration:75ms;opacity:.32}.mdc-button--raised.mdc-ripple-upgraded,.mdc-button--unelevated.mdc-ripple-upgraded{--mdc-ripple-fg-opacity: 0.32}.mdc-button--raised{box-shadow:0px 3px 1px -2px rgba(0, 0, 0, 0.2),0px 2px 2px 0px rgba(0, 0, 0, 0.14),0px 1px 5px 0px rgba(0,0,0,.12);transition:box-shadow 280ms cubic-bezier(0.4, 0, 0.2, 1)}.mdc-button--raised:hover,.mdc-button--raised:focus{box-shadow:0px 2px 4px -1px rgba(0, 0, 0, 0.2),0px 4px 5px 0px rgba(0, 0, 0, 0.14),0px 1px 10px 0px rgba(0,0,0,.12)}.mdc-button--raised:active{box-shadow:0px 5px 5px -3px rgba(0, 0, 0, 0.2),0px 8px 10px 1px rgba(0, 0, 0, 0.14),0px 3px 14px 2px rgba(0,0,0,.12)}.mdc-button--raised:disabled{box-shadow:0px 0px 0px 0px rgba(0, 0, 0, 0.2),0px 0px 0px 0px rgba(0, 0, 0, 0.14),0px 0px 0px 0px rgba(0,0,0,.12)}.mdc-button--outlined{border-style:solid;padding:8px 8px 8px 8px;border-width:1px}.mdc-button--outlined:disabled{border-color:rgba(0,0,0,.37)}.mdc-button--outlined:not(:disabled){border-color:#6200ee;border-color:var(--mdc-button-outline-color, #6200ee)}.mdc-button--dense{height:32px;font-size:.8125rem}.material-icons{font-family:var(--mdc-icon-font, "Material Icons");font-weight:normal;font-style:normal;font-size:var(--mdc-icon-size, 24px);line-height:1;letter-spacing:normal;text-transform:none;display:inline-block;white-space:nowrap;word-wrap:normal;direction:ltr;-webkit-font-feature-settings:"liga";-webkit-font-smoothing:antialiased}:host{display:inline-flex;outline:none}.mdc-button{flex:1}

        .keypad_display {
          background: #35758c;
          border-radius: 10px;
          width: 200px;
          height: 50px;
          margin: auto;
          padding-top: 15px;
          padding-bottom: 10px;
          margin-bottom: 20px;
        }
        .keypad_state {
          padding-left: 30px;
          font-size: calc(var(--base-unit) * 1);
          line-height: 1.1;
          color: black;
          font-family: monospace;
        }
        #keypad_state1 {
          padding-bottom: 10px;
          white-space: pre-wrap;
        }
        #keypad_state2 {
          white-space: pre-wrap;
        } 

        .pad {
          display: flex;
          justify-content: center;
        }
        .pad div {
          display: flex;
          flex-direction: column;
        }

        .mdc-button {
          margin-top: 8px;
          margin-right: 8px;
          margin-bottom: 8px;
          margin-left: 8px;
        }

        .quickset {
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

    </style>
    
    <ha-card header="[[_title]]">
        <div id="zoom" style="[[_scale]]">
          <div class='flex-container' on-click="stopPropagation">
              <div class='keypad'>

                <template is='dom-if' if='{{_view_display}}'>
                  <div class="keypad_display">
                   <div class="keypad_state" id="keypad_state0">[[_line0]]</div>
                    <div class="keypad_state" id="keypad_state1">[[_line1]]</div>
                    <div class="keypad_state" id="keypad_state2">[[_line2]]</div>
                    <div class="keypad_state" id="keypad_state3">[[_line3]]</div>
                  </div>
                </template>

                <template is='dom-if' if='{{_view_pad}}'>
                  <div class="pad">
                    <div>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="1"
                        on-click='setState'
                        title='Unset'>1
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="4"
                        on-click='setState'
                        title='Unset'>4
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="7"
                        on-click='setState'
                        title='Unset'>7
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="*"
                        on-click='setState'
                        title='Unset'>*
                      </button>
                    </div>

                    <div>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="2"
                        on-click='setState'
                        title='Unset'>2
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="5"
                        on-click='setState'
                        title='Unset'>5
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="8"
                        on-click='setState'
                        title='Unset'>8
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="0"
                        on-click='setState'
                        title='Unset'>0
                      </button>
                    </div>

                    <div>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="3"
                        on-click='setState'
                        title='Unset'>3
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="6"
                        on-click='setState'
                        title='Unset'>6
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="9"
                        on-click='setState'
                        title='Unset'>9
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="#"
                        on-click='setState'
                        title='Unset'>#
                      </button>
                    </div>

 <div>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="A"
                        on-click='setState'
                        title='Unset'>[[_button_A]]
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="B"
                        on-click='setState'
                        title='Unset'>[[_button_B]]
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="C"
                        on-click='setState'
                        title='Unset'>[[_button_C]]
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="D"
                        on-click='setState'
                        title='Unset'>[[_button_D]]
                      </button>
                    </div>
                    
                  </div>
                </template>

                <template is='dom-if' if='{{_view_quickset}}'>
                    <div class="quickset">
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="A"
                        on-click='setState'
                        title='Unset'>Timed set
                      </button>
                      <button
                        class='mdc-button mdc-button--outlined'
                        toggles state="B"
                        on-click='setState'
                        title='Unset'>Part set
                      </button>
                    </div>
                </template>

                <audio id="exitsound1" loop>
                  <source src="/local/beep.mp3" type="audio/mpeg">
                </audio>
                <audio id="exitsound2" loop>
                  <source src="/local/beep_fast.mp3" type="audio/mpeg">
                </audio>
                <audio id="chime">
                  <source src="/local/ding_dong.mp3" type="audio/mpeg">
                </audio>
              </div>
          </div>
      </div>
    </ha-card>
    `;
    
  }

  static get properties() {
    return {
        _config: Object,
        _title: String,
     
        _kpdline0: {
          type: Object,
          observer: 'displayChanged'
        },
        _kpdline1: {
          type: Object,
          observer: 'displayChanged'
        },
        _kpdline2: {
          type: Object,
          observer: 'displayChanged'
        },
        _kpdline3: {
          type: Object,
          observer: 'displayChanged'
        },
        _kpdbeep: {
          type: Object,
          observer: 'beepChanged'
        },
        _kpdsetting: {
          type: Object,
          observer: 'settingChanged'
        },
     
        _kpdservice: String,
        _kpdservicekey: String,
        _button_A: String,
        _button_B: String,
        _button_C: String,
        _button_D: String, 
        _cmd_A: String,
        _cmd_B: String,
        _cmd_C: String,
        _cmd_D: String,  
        _line0: String,        
        _line1: String,
        _line2: String,
        _line3: String,
        _view_display: Boolean,
        _view_pad: Boolean,
        _view_quickset: Boolean,
        _scale: String,
    }
  }

  setConfig(config) {
      if (!config.unique_id) throw new Error('You need to define a unique_id');
      if (!config.kpd_line1) throw new Error('You need to define a kpd_line1');
      if (!config.kpd_line2) throw new Error('You need to define a kpd_line2');
      if (!config.kpd_service) throw new Error('You need to define a keypad service');
     
      
      this.setProperties({
        _config: config,
        _title: config.title,
        _kpdservice: config.kpd_service,
        _kpdservicekey: (config.kpd_servicekey !=null)?config.kpd_servicekey:"keys",
        _view_display: (config.view_display != null) ? config.view_display : true,
        _view_pad: (config.view_pad != null) ? config.view_pad : true,
        _view_quickset: (config.view_quickset != null) ? config.view_quickset : false,
        _scale: (config.scale != null) ? "transform-origin: 0 0; zoom: "+config.scale+"; -moz-transform: scale("+config.scale+");" : "1",
        _button_A: (config.button_A != null)?config.button_A:"A",
        _button_B: (config.button_B != null)?config.button_B:"B",
        _button_C: (config.button_C != null)?config.button_C:"C",
        _button_D: (config.button_D != null)?config.button_D:"D",
        _cmd_A: (config.cmd_A != null)?config.cmd_A:"",
        _cmd_B: (config.cmd_B != null)?config.cmd_B:"",
        _cmd_C: (config.cmd_C != null)?config.cmd_C:"",
        _cmd_D: (config.cmd_D != null)?config.cmd_D:""        
        
      });
  }

  set hass(hass) {
    this._hass = hass;
    
    this._kpdline0 =  this._hass.states[this._config.kpd_line0];
    this._kpdline1 = this._hass.states[this._config.kpd_line1];
    this._kpdline2 = this._hass.states[this._config.kpd_line2];
    this._kpdline3 =  this._hass.states[this._config.kpd_line3];
    this._kpdbeep = this._hass.states[this._config.beep];
    this._kpdsetting = this._hass.states[this._config.setting];

  }

  displayChanged() {
    //let state1 = "";
    //let state2 = "";

   // for (let i = 0; i < this._kpdline1.state.length; i++) state1 += this._translateChar(this._kpdline1.state[i]);
    //for (let i = 0; i < this._kpdline2.state.length; i++) state2 += this._translateChar(this._kpdline2.state[i]);

    this.setProperties({
      //_line1: state1,
     // _line2: state2,
      _line1:( this._kpdline1 == null ?"": this._kpdline1.state),
      _line2:( this._kpdline2 == null ?"": this._kpdline2.state),
      _line3:( this._kpdline3 == null ?"": this._kpdline3.state),
      _line0:( this._kpdline0 == null ?"": this._kpdline0.state)
    });
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
    } else if (this._kpdbeep.state == "3") {
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

  settingChanged() {

  }

  _translateChar(c) {
    // if (c.match('à') !== null ) return '<span class="blink">' + c + '</span>';
    // if (c.match('á') !== null ) return '<span class="under">' + c + '</span>';
  //  if (c.match('è') !== null ) return '░';
   // if (c.match('é') !== null ) return '▓';

    return c;
  }

  stopPropagation(e) {
      e.stopPropagation();
  }

  setState(e) {
      var newState = {};
      var key=e.currentTarget.getAttribute('state');
      
     switch (key) {
         case 'A': key=this._cmd_A; break;
         case 'B': key=this._cmd_B; break;
         case 'C': key=this._cmd_C; break;
         case 'D': key=this._cmd_D; break;
 
     }
     
     newState[this._kpdservicekey]=key ;
     this._hass.callService('ESPHome', this._kpdservice,newState);
     // this._hass.callService('ESPHome', 'vistaalarmtest_alarm_keypress', { "keys":newState      });
  }

  getCardSize() {
      let size = 2;
      if (this._config.view_pad) size += 4;     // 550px - 190px / 50
      return size;
  }

}

customElements.define('alarm-keypad-card', AlarmKeypadCard);
   