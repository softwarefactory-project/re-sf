let greet = name => print_endline("Hello " ++ name);
let main = () => {
  // Usage
  Clap.description("Demo program with Clap.");
  let name = Clap.mandatory_string(~long="name", ());
  Clap.close();
  // Say hi o/
  greet(name);
};

main();
