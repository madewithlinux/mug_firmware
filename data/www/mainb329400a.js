function t(){}function e(t){return t()}function n(){return Object.create(null)}function o(t){t.forEach(e)}function r(t){return"function"==typeof t}function s(t,e){return t!=t?e==e:t!==e||t&&"object"==typeof t||"function"==typeof t}function l(e){return e&&r(e.destroy)?e.destroy:t}function c(t,e){t.appendChild(e)}function a(t,e,n){const o=function(t){if(!t)return document;const e=t.getRootNode?t.getRootNode():t.ownerDocument;if(e&&e.host)return e;return t.ownerDocument}(t);if(!o.getElementById(e)){const t=f("style");t.id=e,t.textContent=n,function(t,e){c(t.head||t,e)}(o,t)}}function i(t,e,n){t.insertBefore(e,n||null)}function u(t){t.parentNode.removeChild(t)}function f(t){return document.createElement(t)}function d(t){return document.createTextNode(t)}function p(){return d(" ")}function m(t,e,n,o){return t.addEventListener(e,n,o),()=>t.removeEventListener(e,n,o)}function h(t){return function(e){return e.preventDefault(),t.call(this,e)}}function v(t){return function(e){return e.stopPropagation(),t.call(this,e)}}function g(t,e,n){null==n?t.removeAttribute(e):t.getAttribute(e)!==n&&t.setAttribute(e,n)}function b(t){return""===t?null:+t}function _(t,e){e=""+e,t.wholeText!==e&&(t.data=e)}function y(t,e){t.value=null==e?"":e}function x(t,e,n,o){t.style.setProperty(e,n,o?"important":"")}let $;function k(t){$=t}function z(){if(!$)throw new Error("Function called outside component initialization");return $}const w=[],C=[],S=[],E=[],O=Promise.resolve();let N=!1;function j(t){S.push(t)}const F=new Set;let M=0;function A(){const t=$;do{for(;M<w.length;){const t=w[M];M++,k(t),L(t.$$)}for(k(null),w.length=0,M=0;C.length;)C.pop()();for(let t=0;t<S.length;t+=1){const e=S[t];F.has(e)||(F.add(e),e())}S.length=0}while(w.length);for(;E.length;)E.pop()();N=!1,F.clear(),k(t)}function L(t){if(null!==t.fragment){t.update(),o(t.before_update);const e=t.dirty;t.dirty=[-1],t.fragment&&t.fragment.p(t.ctx,e),t.after_update.forEach(j)}}const J=new Set;let P;function T(){P={r:0,c:[],p:P}}function B(){P.r||o(P.c),P=P.p}function D(t,e){t&&t.i&&(J.delete(t),t.i(e))}function R(t,e,n,o){if(t&&t.o){if(J.has(t))return;J.add(t),P.c.push((()=>{J.delete(t),o&&(n&&t.d(1),o())})),t.o(e)}}function I(t,e){const n=e.token={};function o(t,o,r,s){if(e.token!==n)return;e.resolved=s;let l=e.ctx;void 0!==r&&(l=l.slice(),l[r]=s);const c=t&&(e.current=t)(l);let a=!1;e.block&&(e.blocks?e.blocks.forEach(((t,n)=>{n!==o&&t&&(T(),R(t,1,1,(()=>{e.blocks[n]===t&&(e.blocks[n]=null)})),B())})):e.block.d(1),c.c(),D(c,1),c.m(e.mount(),e.anchor),a=!0),e.block=c,e.blocks&&(e.blocks[o]=c),a&&A()}if((r=t)&&"object"==typeof r&&"function"==typeof r.then){const n=z();if(t.then((t=>{k(n),o(e.then,1,e.value,t),k(null)}),(t=>{if(k(n),o(e.catch,2,e.error,t),k(null),!e.hasCatch)throw t})),e.current!==e.pending)return o(e.pending,0),!0}else{if(e.current!==e.then)return o(e.then,1,e.value,t),!0;e.resolved=t}var r}function U(t,e,n){const o=e.slice(),{resolved:r}=t;t.current===t.then&&(o[t.value]=r),t.current===t.catch&&(o[t.error]=r),t.block.p(o,n)}function q(t,n,s,l){const{fragment:c,on_mount:a,on_destroy:i,after_update:u}=t.$$;c&&c.m(n,s),l||j((()=>{const n=a.map(e).filter(r);i?i.push(...n):o(n),t.$$.on_mount=[]})),u.forEach(j)}function H(t,e){const n=t.$$;null!==n.fragment&&(o(n.on_destroy),n.fragment&&n.fragment.d(e),n.on_destroy=n.fragment=null,n.ctx=[])}function G(t,e){-1===t.$$.dirty[0]&&(w.push(t),N||(N=!0,O.then(A)),t.$$.dirty.fill(0)),t.$$.dirty[e/31|0]|=1<<e%31}function K(e,r,s,l,c,a,i,f=[-1]){const d=$;k(e);const p=e.$$={fragment:null,ctx:null,props:a,update:t,not_equal:c,bound:n(),on_mount:[],on_destroy:[],on_disconnect:[],before_update:[],after_update:[],context:new Map(r.context||(d?d.$$.context:[])),callbacks:n(),dirty:f,skip_bound:!1,root:r.target||d.$$.root};i&&i(p.root);let m=!1;if(p.ctx=s?s(e,r.props||{},((t,n,...o)=>{const r=o.length?o[0]:n;return p.ctx&&c(p.ctx[t],p.ctx[t]=r)&&(!p.skip_bound&&p.bound[t]&&p.bound[t](r),m&&G(e,t)),n})):[],p.update(),m=!0,o(p.before_update),p.fragment=!!l&&l(p.ctx),r.target){if(r.hydrate){const t=function(t){return Array.from(t.childNodes)}(r.target);p.fragment&&p.fragment.l(t),t.forEach(u)}else p.fragment&&p.fragment.c();r.intro&&D(e.$$.fragment),q(e,r.target,r.anchor,r.customElement),A()}k(d)}class Q{$destroy(){H(this,1),this.$destroy=t}$on(t,e){const n=this.$$.callbacks[t]||(this.$$.callbacks[t]=[]);return n.push(e),()=>{const t=n.indexOf(e);-1!==t&&n.splice(t,1)}}$set(t){var e;this.$$set&&(e=t,0!==Object.keys(e).length)&&(this.$$.skip_bound=!0,this.$$set(t),this.$$.skip_bound=!1)}}async function V(){const t=await fetch("/api/state"),e=await t.json();if(t.ok)return e;throw new Error(e)}const W=(t,e)=>{const n=()=>{t&&(t.value=t.valueAsNumber.toFixed(e))};return n(),t.addEventListener("change",n),{destroy(){t.removeEventListener("change",n)}}};function X(t){a(t,"svelte-avecsz",'.config.svelte-avecsz{display:grid;grid-template-columns:1fr 1fr;gap:10px}@media(min-width: 600px){.config.svelte-avecsz{max-width:max-content}}.num.svelte-avecsz{justify-self:end}.config.svelte-avecsz,label.svelte-avecsz,input.svelte-avecsz{font-size:1.2rem}label.svelte-avecsz{justify-self:stretch;align-self:stretch}input.svelte-avecsz{justify-self:end}input[type="number"].svelte-avecsz{width:140px;text-align:end}button.svelte-avecsz{margin:5px;padding:20px 10px;font-size:1.2rem}')}function Y(t){let e,n,o={ctx:t,current:null,token:null,hasCatch:!0,pending:et,then:tt,catch:Z,error:20};return I(n=t[2],o),{c(){e=d(""),o.block.c()},m(t,n){i(t,e,n),o.block.m(t,o.anchor=n),o.mount=()=>e.parentNode,o.anchor=e},p(e,r){t=e,o.ctx=t,4&r&&n!==(n=t[2])&&I(n,o)||U(o,t,r)},d(t){t&&u(e),o.block.d(t),o.token=null,o=null}}}function Z(t){let e,n,o,r=t[20].message+"";return{c(){e=f("p"),n=d("error: "),o=d(r),x(e,"color","red")},m(t,r){i(t,e,r),c(e,n),c(e,o)},p(t,e){4&e&&r!==(r=t[20].message+"")&&_(o,r)},d(t){t&&u(e)}}}function tt(e){return{c:t,m:t,p:t,d:t}}function et(e){let n;return{c(){n=d("...loading")},m(t,e){i(t,n,e)},p:t,d(t){t&&u(n)}}}function nt(e){let n,r,s,a,x,$,k,z,w,C,S,E,O,N,j,F,M,A,L,J,P,T,B,D,R,I,U,q,H,G,K,Q,V,X,Z,tt,et,nt,ot,rt,st,lt,ct,at,it,ut,ft,dt,pt,mt,ht,vt,gt,bt,_t,yt,xt,$t,kt,zt,wt,Ct,St=e[0].is_heater_on?"ON":"OFF",Et=e[0].pwm_output+"",Ot=e[0].current_temp_f.toFixed(4)+"",Nt=JSON.stringify(e[0],void 0," ")+"",jt=e[2]&&Y(e);return{c(){n=f("form"),r=f("span"),r.textContent="uptime",s=f("span"),a=d(e[3]),x=d(":"),$=d(e[4]),k=d(":"),z=d(e[5]),w=p(),C=f("span"),C.textContent="heater state",S=p(),E=f("span"),O=d(St),N=d(" (level="),j=d(Et),F=d(")"),M=p(),A=f("span"),A.textContent="current_temp_f",L=f("span"),J=d(Ot),P=p(),T=f("label"),T.textContent="target_temp",B=p(),D=f("input"),R=p(),I=f("label"),I.textContent="threshold_temp",U=p(),q=f("input"),H=p(),G=f("label"),G.textContent="temp_hot",K=p(),Q=f("input"),V=p(),X=f("label"),X.textContent="temp_cold",Z=p(),tt=f("input"),et=p(),nt=f("label"),nt.textContent="refresh_interval",ot=f("input"),rt=p(),st=f("label"),st.textContent="kp",lt=f("input"),ct=p(),at=f("label"),at.textContent="ki",it=f("input"),ut=p(),ft=f("label"),ft.textContent="kd",dt=f("input"),pt=p(),mt=f("label"),mt.textContent="led_brightness",ht=p(),vt=f("input"),gt=p(),bt=f("button"),bt.textContent="reset",_t=p(),yt=f("button"),yt.textContent="save",xt=p(),jt&&jt.c(),$t=p(),kt=f("pre"),zt=d(Nt),g(s,"class","num svelte-avecsz"),g(E,"class","num svelte-avecsz"),g(L,"class","num svelte-avecsz"),g(T,"for","target_temp_f"),g(T,"class","svelte-avecsz"),g(D,"id","target_temp_f"),g(D,"type","number"),g(D,"class","svelte-avecsz"),g(I,"for","threshold_temp_f"),g(I,"class","svelte-avecsz"),g(q,"id","threshold_temp_f"),g(q,"type","number"),g(q,"class","svelte-avecsz"),g(G,"for","temp_hot"),g(G,"class","svelte-avecsz"),g(Q,"id","temp_hot"),g(Q,"type","number"),g(Q,"class","svelte-avecsz"),g(X,"for","temp_cold"),g(X,"class","svelte-avecsz"),g(tt,"id","temp_cold"),g(tt,"type","number"),g(tt,"class","svelte-avecsz"),g(nt,"for","refresh_interval"),g(nt,"class","svelte-avecsz"),g(ot,"id","refresh_interval"),g(ot,"type","number"),g(ot,"class","svelte-avecsz"),g(st,"for","kp"),g(st,"class","svelte-avecsz"),g(lt,"id","kp"),g(lt,"type","number"),g(lt,"step","0.1"),g(lt,"class","svelte-avecsz"),g(at,"for","ki"),g(at,"class","svelte-avecsz"),g(it,"id","ki"),g(it,"type","number"),g(it,"step","0.1"),g(it,"class","svelte-avecsz"),g(ft,"for","kd"),g(ft,"class","svelte-avecsz"),g(dt,"id","kd"),g(dt,"type","number"),g(dt,"step","0.1"),g(dt,"class","svelte-avecsz"),g(mt,"for","led_brightness"),g(mt,"class","svelte-avecsz"),g(vt,"id","led_brightness"),g(vt,"type","number"),g(vt,"min","0"),g(vt,"max","255"),g(vt,"class","svelte-avecsz"),g(bt,"type","button"),g(bt,"class","svelte-avecsz"),g(yt,"type","submit"),g(yt,"class","svelte-avecsz"),g(n,"class","config svelte-avecsz")},m(t,o){i(t,n,o),c(n,r),c(n,s),c(s,a),c(s,x),c(s,$),c(s,k),c(s,z),c(n,w),c(n,C),c(n,S),c(n,E),c(E,O),c(E,N),c(E,j),c(E,F),c(n,M),c(n,A),c(n,L),c(L,J),c(n,P),c(n,T),c(n,B),c(n,D),y(D,e[1].target_temp_f),c(n,R),c(n,I),c(n,U),c(n,q),y(q,e[1].threshold_temp_f),c(n,H),c(n,G),c(n,K),c(n,Q),y(Q,e[1].temp_hot),c(n,V),c(n,X),c(n,Z),c(n,tt),y(tt,e[1].temp_cold),c(n,et),c(n,nt),c(n,ot),y(ot,e[1].refresh_interval),c(n,rt),c(n,st),c(n,lt),y(lt,e[1].kp),c(n,ct),c(n,at),c(n,it),y(it,e[1].ki),c(n,ut),c(n,ft),c(n,dt),y(dt,e[1].kd),c(n,pt),c(n,mt),c(n,ht),c(n,vt),y(vt,e[1].led_brightness),c(n,gt),c(n,bt),c(n,_t),c(n,yt),i(t,xt,o),jt&&jt.m(t,o),i(t,$t,o),i(t,kt,o),c(kt,zt),wt||(Ct=[m(D,"input",e[7]),l(W.call(null,D,2)),m(q,"input",e[8]),l(W.call(null,q,2)),m(Q,"input",e[9]),l(W.call(null,Q,1)),m(tt,"input",e[10]),l(W.call(null,tt,1)),m(ot,"input",e[11]),m(lt,"input",e[12]),l(W.call(null,lt,2)),m(it,"input",e[13]),l(W.call(null,it,2)),m(dt,"input",e[14]),l(W.call(null,dt,2)),m(vt,"input",e[15]),m(bt,"click",v(h(e[16]))),m(yt,"click",v(h(e[6]))),m(n,"submit",v(h(e[6])),!0)],wt=!0)},p(t,[e]){8&e&&_(a,t[3]),16&e&&_($,t[4]),32&e&&_(z,t[5]),1&e&&St!==(St=t[0].is_heater_on?"ON":"OFF")&&_(O,St),1&e&&Et!==(Et=t[0].pwm_output+"")&&_(j,Et),1&e&&Ot!==(Ot=t[0].current_temp_f.toFixed(4)+"")&&_(J,Ot),2&e&&b(D.value)!==t[1].target_temp_f&&y(D,t[1].target_temp_f),2&e&&b(q.value)!==t[1].threshold_temp_f&&y(q,t[1].threshold_temp_f),2&e&&b(Q.value)!==t[1].temp_hot&&y(Q,t[1].temp_hot),2&e&&b(tt.value)!==t[1].temp_cold&&y(tt,t[1].temp_cold),2&e&&b(ot.value)!==t[1].refresh_interval&&y(ot,t[1].refresh_interval),2&e&&b(lt.value)!==t[1].kp&&y(lt,t[1].kp),2&e&&b(it.value)!==t[1].ki&&y(it,t[1].ki),2&e&&b(dt.value)!==t[1].kd&&y(dt,t[1].kd),2&e&&b(vt.value)!==t[1].led_brightness&&y(vt,t[1].led_brightness),t[2]?jt?jt.p(t,e):(jt=Y(t),jt.c(),jt.m($t.parentNode,$t)):jt&&(jt.d(1),jt=null),1&e&&Nt!==(Nt=JSON.stringify(t[0],void 0," ")+"")&&_(zt,Nt)},i:t,o:t,d(t){t&&u(n),t&&u(xt),jt&&jt.d(t),t&&u($t),t&&u(kt),wt=!1,o(Ct)}}}function ot(t,e,n){let o,r,s,{remoteState:l}=e;const c={};let a=Object.assign({},l),i=Object.assign({},l);let u;return t.$$set=t=>{"remoteState"in t&&n(0,l=t.remoteState)},t.$$.update=()=>{1&t.$$.dirty&&n(5,o=Math.floor(l.millis/1e3%60).toString().padStart(2,"0")),1&t.$$.dirty&&n(4,r=Math.floor(l.millis/6e4%60).toString().padStart(2,"0")),1&t.$$.dirty&&n(3,s=Math.floor(l.millis/36e5).toString().padStart(2,"0")),1&t.$$.dirty&&(t=>{for(const[e,o]of Object.entries(t))c[e]!==document.activeElement&&a[e]===i[e]&&a[e]!==o&&n(1,a[e]=o,a);i=t})(l)},[l,a,u,s,r,o,()=>{const t=Object.assign(Object.assign({},l),a);n(2,u=(async()=>{n(1,a=await async function(t){const e=await fetch("/api/state",{method:"POST",headers:{"Content-Type":"application/json"},body:JSON.stringify(t)});if(e.ok)return await V();throw new Error(await e.text())}(t))})())},function(){a.target_temp_f=b(this.value),n(1,a)},function(){a.threshold_temp_f=b(this.value),n(1,a)},function(){a.temp_hot=b(this.value),n(1,a)},function(){a.temp_cold=b(this.value),n(1,a)},function(){a.refresh_interval=b(this.value),n(1,a)},function(){a.kp=b(this.value),n(1,a)},function(){a.ki=b(this.value),n(1,a)},function(){a.kd=b(this.value),n(1,a)},function(){a.led_brightness=b(this.value),n(1,a)},()=>n(1,a=l)]}class rt extends Q{constructor(t){super(),K(this,t,ot,nt,s,{remoteState:0},X)}}function st(t){a(t,"svelte-d7bdnv",'body{margin:20px 0;padding:8px;box-sizing:border-box;font-family:-apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Oxygen-Sans, Ubuntu, Cantarell,\n      "Helvetica Neue", sans-serif;font-size:1.2rem}main.svelte-d7bdnv{padding-left:1em;padding-right:1em;margin:0 auto}@media(max-width: 1000px){body{padding:0}}')}function lt(t){let e,n,o,r=t[5].message+"";return{c(){e=f("p"),n=d("error: "),o=d(r),x(e,"color","red")},m(t,r){i(t,e,r),c(e,n),c(e,o)},p(t,e){1&e&&r!==(r=t[5].message+"")&&_(o,r)},d(t){t&&u(e)}}}function ct(e){return{c:t,m:t,p:t,d:t}}function at(e){let n;return{c(){n=d("...loading")},m(t,e){i(t,n,e)},p:t,d(t){t&&u(n)}}}function it(t){let e,n;return e=new rt({props:{remoteState:t[1]}}),{c(){var t;(t=e.$$.fragment)&&t.c()},m(t,o){q(e,t,o),n=!0},p(t,n){const o={};2&n&&(o.remoteState=t[1]),e.$set(o)},i(t){n||(D(e.$$.fragment,t),n=!0)},o(t){R(e.$$.fragment,t),n=!1},d(t){H(e,t)}}}function ut(t){let e,n,o,r,s,l,a,d,m,h={ctx:t,current:null,token:null,hasCatch:!0,pending:at,then:ct,catch:lt,value:1,error:5};I(a=t[0],h);let v=t[1]&&it(t);return{c(){e=f("main"),n=f("h1"),n.textContent="Mug",o=p(),r=f("p"),s=f("a"),s.textContent="refresh",l=p(),h.block.c(),d=p(),v&&v.c(),g(s,"href","/"),g(e,"class","svelte-d7bdnv")},m(t,a){i(t,e,a),c(e,n),c(e,o),c(e,r),c(r,s),c(r,l),h.block.m(r,h.anchor=null),h.mount=()=>r,h.anchor=null,c(e,d),v&&v.m(e,null),m=!0},p(n,[o]){t=n,h.ctx=t,1&o&&a!==(a=t[0])&&I(a,h)||U(h,t,o),t[1]?v?(v.p(t,o),2&o&&D(v,1)):(v=it(t),v.c(),D(v,1),v.m(e,null)):v&&(T(),R(v,1,1,(()=>{v=null})),B())},i(t){m||(D(v),m=!0)},o(t){R(v),m=!1},d(t){t&&u(e),h.block.d(),h.token=null,h=null,v&&v.d()}}}function ft(t,e,n){let o;let r,s=async function(){return n(1,o=await V()),o}();return window.EventSource&&(r=new EventSource("/api/events"),r.onopen=t=>{console.log("Events Connected")},r.onmessage=t=>{console.log(t)},r.addEventListener("state",(t=>{n(1,o=JSON.parse(t.data))}),!1)),[s,o]}const dt=new class extends Q{constructor(t){super(),K(this,t,ft,ut,s,{},st)}}({target:document.body});export{dt as default};