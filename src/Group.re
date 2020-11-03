[@decco]
type t = {
  name: string,
  description: option(string),
  members: list(string),
};

let decode = t_decode;
let encode = t_encode;
