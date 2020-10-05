type tenant = {
  name: string,
  default_connection: option(string),
  description: option(string),
  url: string,
};

let parse = json => {
  Json.Decode.{
    name: json |> field("name", string),
    default_connection:
      json |> optional(field("default-connection", string)),
    url: json |> field("url", string),
    description: json |> optional(field("description", string)),
  };
};
