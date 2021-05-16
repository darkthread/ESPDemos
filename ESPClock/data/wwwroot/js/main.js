var debHandlers = {};
function debounce(key, cb, delay) {
    clearTimeout(debHandlers[key] || 0);
    debHandlers[key] = setTimeout(cb, delay);
}

var vm = Vue.createApp({
    data() {
        return {
            LedSwitch: true,
            ClockMode: true,
            Brightness: 1,
            EnableScroll: false,
            Message: "darkthread",
            Speed: 50
        }
    },
    computed: {
    
    },
    watch: {
        LedSwitch: (v) => $.post('/led/switch?v=' + (v ? "on" : "off")),
        EnableScroll: function(v) { $.post("/toggle-scroll", { v: this.EnableScroll ? "Y" : "N" }) },
        ClockMode: function(v) { $.post("/set-mode", { m: this.ClockMode ? "clock" : "marquee" }) },
        Speed: (newValue) => {
            debounce("set-speed", function() {
                $.post("/set-scroll-delay", { d: 100 - newValue });
            }, 250);
        },
        Brightness: (v) => {
            debounce("set-brightness", function() {
                $.post("/set-brightness", { l: v });
            }, 250);
        }
    },
    methods: {
        SendMessage: function() {
            $.post("/show-message", { m: this.Message });
        }
    }

}).mount('#app');
