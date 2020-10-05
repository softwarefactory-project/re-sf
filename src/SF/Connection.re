type connection = {
  name: string,
  connection_type: string,
  base_url: option(string),
};

let parse = json => {
  Json.Decode.{
    connection_type: json |> field("type", string),
    name: json |> field("name", string),
    base_url: json |> optional(field("base-url", string)),
  };
};
