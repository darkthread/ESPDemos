var debHandlers = {};
function debounce(key, cb, delay) {
    clearTimeout(debHandlers[key] || 0);
    debHandlers[key] = setTimeout(cb, delay);
}

var vm = Vue.createApp({
    data() {
        return {
            LedSwitch: true,
            Mode: "clock",
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
        },
        StartTimer: function() {
            $.post("/start-timer");
        },
        StopTimer: function() {
            $.post("/stop-timer");
        },
        SetMode: function(mode) {
            this.Mode = mode;
            $.post("/set-mode", { m: mode });
        }
    }

}).mount('#app');
