var debHandlers = {};
function debounce(key, cb, delay) {
    clearTimeout(debHandlers[key] || 0);
    debHandlers[key] = setTimeout(cb, delay);
}

var vm = Vue.createApp({
    data() {
        return {
            LedSwitch: true,
            Text: "test",
            Message: "Hello World!",
            Speed: 50
        }
    },
    computed: {
    
    },
    watch: {
        LedSwitch: (v) => $.post('/led/switch?v=' + (v ? "on" : "off")),
        Speed: (newValue) => {
            debounce("set-speed", function() {
                $.post("/set-scroll-delay", { d: 100 - newValue });
            }, 800);
        }
    },
    methods: {
        SendMessage: function() {
            $.post("/show-message", { m: this.Message });
        }
    }

}).mount('#app');
